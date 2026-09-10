#pragma once
#include <concurrency/blocking_queue.hpp>
#include <datasink/idatasink/idatasink.hpp>

#include <atomic>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <thread>
#include <vector>

class SharedSinkPool;

// One queue per Context. Messages never capture the Context or anything it
// owns, so a mailbox left in a pool's run queue after its Context has been
// destroyed is still safe to drain.
struct Message {
  std::string data;                             // valid iff barrier == nullptr
  std::shared_ptr<std::promise<void>> barrier;  // non-null => flush barrier
};

// A per-Context FIFO queue that gets scheduled onto a SharedSinkPool's
// worker threads. A mailbox is enqueued on the pool's run queue at most
// once at a time, so at most one worker ever drains it concurrently:
// messages posted to one mailbox are delivered in FIFO order even though
// the worker threads are shared across many mailboxes.
class Mailbox final : public std::enable_shared_from_this<Mailbox> {
public:
  void post(Message message);

  // Posts a barrier and blocks until every message queued ahead of it has
  // been delivered. Must not be called from a pool worker thread: the
  // barrier can only be resolved by a worker draining this mailbox, so a
  // worker waiting on its own barrier would deadlock.
  void drain();

private:
  friend class SharedSinkPool;

  explicit Mailbox(SharedSinkPool& pool) : pool_(&pool) {}

  // Delivers up to a bounded batch of queued messages, then reschedules
  // itself if more work arrived while it was running. Never throws: a
  // failing write must not crash a shared worker thread or leave a
  // flush() caller blocked forever.
  void runBatch(IDataSink& sink) noexcept;
  static void deliver(IDataSink& sink, Message message) noexcept;

  SharedSinkPool* pool_;
  BlockingQueue<Message> queue_;
  std::atomic<bool> scheduled_{false};
};

// Owns a fixed set of worker threads and leaf sinks (one sink per thread),
// shared across every Context. Each Context gets its own Mailbox(es); the
// pool only decides which thread drains a ready mailbox next.
class SharedSinkPool {
public:
  using SinkFactory = std::function<std::unique_ptr<IDataSink>(std::size_t worker_index)>;

  SharedSinkPool(SinkFactory factory, std::size_t thread_count);
  ~SharedSinkPool();

  SharedSinkPool(const SharedSinkPool&) = delete;
  SharedSinkPool& operator=(const SharedSinkPool&) = delete;

  std::shared_ptr<Mailbox> createMailbox();

private:
  friend class Mailbox;

  // Non-throwing: returns false instead of propagating BlockingQueue's
  // "push to a closed queue" exception when called while the pool is
  // shutting down (e.g. a Context torn down during static destruction).
  bool schedule(std::shared_ptr<Mailbox> mailbox);

  void workerLoop(IDataSink& sink);

  std::vector<std::unique_ptr<IDataSink>> sinks_;
  std::vector<std::thread> threads_;
  BlockingQueue<std::shared_ptr<Mailbox>> ready_;
  std::atomic<bool> stopped_{false};
};
