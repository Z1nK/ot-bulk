#include <datasink/pool-data-sink/pool_data_sink.hpp>
#include <datasink/shared-sink-pool/shared_sink_pool.hpp>

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {

// Records writes into storage owned by the test, not by the sink itself, so
// the recorded data stays valid even after the SharedSinkPool (and the
// FakeDataSink instances it owns) has been destroyed.
class FakeDataSink : public IDataSink {
public:
  explicit FakeDataSink(std::vector<std::string>* out) : out_(out) {}

  void write(std::string_view data) override { out_->emplace_back(data); }

private:
  std::vector<std::string>* out_;
};

}  // namespace

// With mailbox scheduling, a mailbox's messages can run on whichever shared
// thread picks it up next, so unlike a dedicated-thread-per-worker design
// there is no fixed mailbox -> leaf-sink affinity to assert on. What the
// adapter still guarantees is that every write is delivered exactly once,
// round-robined across its mailboxes.
TEST(PoolDataSinkTest, WriteDeliversEveryBlockExactlyOnce) {
  std::vector<std::string> recorded_a;
  std::vector<std::string> recorded_b;

  SharedSinkPool pool(
      [&](std::size_t worker_index) {
        return std::make_unique<FakeDataSink>(worker_index == 0 ? &recorded_a : &recorded_b);
      },
      2);

  PoolDataSink sink({pool.createMailbox(), pool.createMailbox()});

  std::vector<std::string> blocks{"block0", "block1", "block2", "block3", "block4"};
  for (const auto& block : blocks) {
    sink.write(block);
  }

  sink.flush();

  std::vector<std::string> delivered = recorded_a;
  delivered.insert(delivered.end(), recorded_b.begin(), recorded_b.end());
  std::sort(delivered.begin(), delivered.end());

  std::vector<std::string> expected = blocks;
  std::sort(expected.begin(), expected.end());

  EXPECT_EQ(delivered, expected);
}

TEST(PoolDataSinkTest, DestructorDrainsPendingWrites) {
  std::vector<std::string> recorded;

  SharedSinkPool pool([&recorded](std::size_t) { return std::make_unique<FakeDataSink>(&recorded); }, 1);

  {
    PoolDataSink sink({pool.createMailbox()});

    sink.write("a");
    sink.write("b");
    sink.write("c");
    // PoolDataSink's destructor only drops its shared_ptr<Mailbox>
    // references; it does not block draining them (that's what makes
    // Context teardown non-blocking). The single-threaded pool below still
    // drains this mailbox before it can reach a barrier posted afterward,
    // since the pool has exactly one worker draining a single shared FIFO
    // of ready mailboxes in the order they were scheduled.
  }

  pool.createMailbox()->drain();

  ASSERT_EQ(recorded.size(), 3u);
  EXPECT_EQ(recorded[0], "a");
  EXPECT_EQ(recorded[1], "b");
  EXPECT_EQ(recorded[2], "c");
}
