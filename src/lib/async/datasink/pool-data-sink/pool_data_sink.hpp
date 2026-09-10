#pragma once
#include <datasink/idatasink/idatasink.hpp>
#include <datasink/shared-sink-pool/shared_sink_pool.hpp>

#include <cstddef>
#include <memory>
#include <vector>

// Adapts one or more SharedSinkPool mailboxes to the IDataSink interface.
// write() round-robins across the mailboxes; flush() is a fan-out barrier
// across all of them. Destruction simply drops the mailboxes: already
// queued messages are still delivered by the pool's worker threads
// afterward, so teardown never blocks on draining a queue.
class PoolDataSink final : public IDataSink {
public:
  explicit PoolDataSink(std::vector<std::shared_ptr<Mailbox>> mailboxes);

  PoolDataSink(const PoolDataSink&) = delete;
  PoolDataSink& operator=(const PoolDataSink&) = delete;

  void write(std::string_view data) override;
  void flush() override;

private:
  std::vector<std::shared_ptr<Mailbox>> mailboxes_;
  std::size_t next_ = 0;
};
