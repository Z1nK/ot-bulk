#include "shared_sink_pool.hpp"

#include <cassert>
#include <exception>
#include <iostream>
#include <utility>

namespace {
// Set for the duration of a pool worker's task loop, so Mailbox::drain()
// can refuse to be called from a worker thread: the barrier it waits on
// can only be resolved by a worker draining this same mailbox, so a worker
// waiting on its own barrier would deadlock.
thread_local bool g_in_pool_worker = false;
}  // namespace

void Mailbox::post(Message message) {
  queue_.push(std::move(message));
  if (!scheduled_.exchange(true)) {
    pool_->schedule(shared_from_this());
  }
}

void Mailbox::drain() {
  assert(!g_in_pool_worker && "Mailbox::drain() called from a pool worker thread");

  auto barrier = std::make_shared<std::promise<void>>();
  std::future<void> done = barrier->get_future();
  post(Message{std::string(), std::move(barrier)});
  done.wait();
}

void Mailbox::deliver(IDataSink& sink, Message message) noexcept {
  try {
    if (message.barrier) {
      sink.flush();
    } else {
      sink.write(message.data);
    }
  } catch (const std::exception& e) {
    try {
      std::cerr << "[shared-sink-pool] sink error: " << e.what() << '\n';
    } catch (...) {
    }
  } catch (...) {
  }

  if (message.barrier) {
    message.barrier->set_value();
  }
}

void Mailbox::runBatch(IDataSink& sink) noexcept {
  constexpr std::size_t kMaxBatch = 64;

  for (std::size_t i = 0; i < kMaxBatch; ++i) {
    auto message = queue_.try_pop();
    if (!message) {
      break;
    }
    deliver(sink, std::move(*message));
  }

  scheduled_.store(false);
  if (!queue_.empty() && !scheduled_.exchange(true)) {
    pool_->schedule(shared_from_this());
  }
}

SharedSinkPool::SharedSinkPool(SinkFactory factory, std::size_t thread_count) {
  sinks_.reserve(thread_count);
  for (std::size_t i = 0; i < thread_count; ++i) {
    sinks_.push_back(factory(i));
  }

  threads_.reserve(thread_count);
  for (std::size_t i = 0; i < thread_count; ++i) {
    IDataSink* sink = sinks_[i].get();
    threads_.emplace_back([this, sink] { workerLoop(*sink); });
  }
}

SharedSinkPool::~SharedSinkPool() {
  stopped_.store(true);
  ready_.close();
  for (auto& thread : threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

std::shared_ptr<Mailbox> SharedSinkPool::createMailbox() {
  return std::shared_ptr<Mailbox>(new Mailbox(*this));
}

bool SharedSinkPool::schedule(std::shared_ptr<Mailbox> mailbox) {
  if (stopped_.load()) {
    return false;
  }
  try {
    ready_.push(std::move(mailbox));
    return true;
  } catch (...) {
    return false;
  }
}

void SharedSinkPool::workerLoop(IDataSink& sink) {
  g_in_pool_worker = true;
  while (auto mailbox = ready_.pop()) {
    try {
      (*mailbox)->runBatch(sink);
    } catch (...) {
    }
  }
}
