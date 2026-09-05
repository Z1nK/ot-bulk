#pragma once
#include <concurrency/blocking_queue.hpp>
#include <datasink/idatasink/idatasink.hpp>

#include <functional>
#include <memory>
#include <thread>

class AsyncDataSink final : public IDataSink {
public:
  explicit AsyncDataSink(std::unique_ptr<IDataSink> sink);
  ~AsyncDataSink() override;

  AsyncDataSink(const AsyncDataSink&) = delete;
  AsyncDataSink& operator=(const AsyncDataSink&) = delete;

  void write(std::string_view data) override;

  void flush() override;

private:
  void processQueue();

  std::unique_ptr<IDataSink> sink_;
  BlockingQueue<std::function<void()>> tasks_;
  std::thread worker_;
};
