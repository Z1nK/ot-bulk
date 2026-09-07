#include "pool_data_sink.hpp"

#include <future>
#include <string>
#include <utility>
#include <vector>

PoolDataSink::PoolDataSink(SinkFactory factory, std::size_t worker_count) {
  workers_.reserve(worker_count);
  for (std::size_t i = 0; i < worker_count; ++i) {
    auto worker = std::make_unique<Worker>();
    worker->sink = factory(i);
    workers_.push_back(std::move(worker));
  }

  for (auto& worker : workers_) {
    Worker* worker_ptr = worker.get();
    worker->thread = std::thread([this, worker_ptr] { processQueue(*worker_ptr); });
  }
}

PoolDataSink::~PoolDataSink() {
  for (auto& worker : workers_) {
    worker->queue.close();
  }
  for (auto& worker : workers_) {
    if (worker->thread.joinable()) {
      worker->thread.join();
    }
  }
}

void PoolDataSink::write(std::string_view data) {
  std::size_t idx = next_++ % workers_.size();
  auto& worker = *workers_[idx];
  IDataSink* sink = worker.sink.get();
  worker.queue.push([sink, data = std::string(data)] { sink->write(data); });
}

void PoolDataSink::flush() {
  std::vector<std::future<void>> futures;
  futures.reserve(workers_.size());

  for (auto& worker : workers_) {
    auto done = std::make_shared<std::promise<void>>();
    futures.push_back(done->get_future());

    IDataSink* sink = worker->sink.get();
    worker->queue.push([sink, done] {
      sink->flush();
      done->set_value();
    });
  }

  for (auto& future : futures) {
    future.wait();
  }
}

void PoolDataSink::processQueue(Worker& worker) {
  while (auto task = worker.queue.pop()) {
    (*task)();
  }
}
