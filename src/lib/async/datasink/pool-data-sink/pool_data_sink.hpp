#pragma once
#include <concurrency/blocking_queue.hpp>
#include <datasink/idatasink/idatasink.hpp>

#include <cstddef>
#include <functional>
#include <memory>
#include <thread>
#include <vector>

class PoolDataSink final : public IDataSink {
public:
  using SinkFactory = std::function<std::unique_ptr<IDataSink>(std::size_t worker_index)>;

  PoolDataSink(SinkFactory factory, std::size_t worker_count);
  ~PoolDataSink() override;

  PoolDataSink(const PoolDataSink&) = delete;
  PoolDataSink& operator=(const PoolDataSink&) = delete;

  void write(std::string_view data) override;
  void flush() override;

private:
  struct Worker {
    std::unique_ptr<IDataSink> sink;
    BlockingQueue<std::function<void()>> queue;
    std::thread thread;
  };

  void processQueue(Worker& worker);

  std::vector<std::unique_ptr<Worker>> workers_;
  std::size_t next_ = 0;
};
