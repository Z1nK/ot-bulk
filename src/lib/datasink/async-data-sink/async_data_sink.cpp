#include "async_data_sink.hpp"

#include <future>
#include <string>
#include <utility>

AsyncDataSink::AsyncDataSink(std::shared_ptr<IDataSink> sink)
    : sink_(std::move(sink)), worker_([this] { processQueue(); }) {}

AsyncDataSink::~AsyncDataSink() {
  tasks_.close();
  if (worker_.joinable()) {
    worker_.join();
  }
}

void AsyncDataSink::write(std::string_view data) {
  tasks_.push([this, data = std::string(data)] { sink_->write(data); });
}

void AsyncDataSink::flush() {
  auto done = std::make_shared<std::promise<void>>();
  std::future<void> done_future = done->get_future();

  tasks_.push([this, done] {
    sink_->flush();
    done->set_value();
  });

  done_future.wait();
}

void AsyncDataSink::processQueue() {
  while (auto task = tasks_.pop()) {
    (*task)();
  }
}
