#include <datasink/shared-sink-pool/shared_sink_pool.hpp>

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

TEST(SharedSinkPoolTest, FactoryIsCalledOncePerThreadWithItsIndex) {
  std::vector<std::size_t> indices;
  std::vector<std::string> discard;

  SharedSinkPool pool(
      [&indices, &discard](std::size_t worker_index) {
        indices.push_back(worker_index);
        return std::make_unique<FakeDataSink>(&discard);
      },
      3);

  ASSERT_EQ(indices.size(), 3u);
  EXPECT_EQ(indices[0], 0u);
  EXPECT_EQ(indices[1], 1u);
  EXPECT_EQ(indices[2], 2u);
}

TEST(SharedSinkPoolTest, PreservesFifoOrderWithinAMailbox) {
  std::vector<std::string> recorded;

  SharedSinkPool pool([&recorded](std::size_t) { return std::make_unique<FakeDataSink>(&recorded); }, 1);

  auto mailbox = pool.createMailbox();
  for (int i = 0; i < 20; ++i) {
    mailbox->post(Message{"block" + std::to_string(i), nullptr});
  }
  mailbox->drain();

  ASSERT_EQ(recorded.size(), 20u);
  for (int i = 0; i < 20; ++i) {
    EXPECT_EQ(recorded[static_cast<std::size_t>(i)], "block" + std::to_string(i));
  }
}

TEST(SharedSinkPoolTest, DroppingLastReferenceStillDeliversQueuedWrites) {
  std::vector<std::string> recorded;

  {
    SharedSinkPool pool([&recorded](std::size_t) { return std::make_unique<FakeDataSink>(&recorded); }, 1);

    auto mailbox = pool.createMailbox();
    mailbox->post(Message{"a", nullptr});
    mailbox->post(Message{"b", nullptr});
    mailbox->post(Message{"c", nullptr});
    // The mailbox shared_ptr goes out of scope here, as it would when a
    // Context is torn down. The pool must still deliver everything already
    // queued before its destructor (below) closes the run queue and joins.
  }

  ASSERT_EQ(recorded.size(), 3u);
  EXPECT_EQ(recorded[0], "a");
  EXPECT_EQ(recorded[1], "b");
  EXPECT_EQ(recorded[2], "c");
}

TEST(SharedSinkPoolTest, ManyMailboxesShareTheSameThreads) {
  constexpr std::size_t kThreadCount = 2;
  constexpr std::size_t kMailboxCount = 3;
  constexpr int kWritesPerMailbox = 5;
  std::vector<std::vector<std::string>> recorded(kThreadCount);

  SharedSinkPool pool(
      [&recorded](std::size_t worker_index) { return std::make_unique<FakeDataSink>(&recorded[worker_index]); },
      kThreadCount);

  std::vector<std::shared_ptr<Mailbox>> mailboxes;
  for (std::size_t m = 0; m < kMailboxCount; ++m) {
    mailboxes.push_back(pool.createMailbox());
  }

  for (std::size_t m = 0; m < mailboxes.size(); ++m) {
    for (int i = 0; i < kWritesPerMailbox; ++i) {
      mailboxes[m]->post(Message{"m" + std::to_string(m) + "-" + std::to_string(i), nullptr});
    }
  }

  for (auto& mailbox : mailboxes) {
    mailbox->drain();
  }

  std::size_t delivered = recorded[0].size() + recorded[1].size();
  EXPECT_EQ(delivered, kMailboxCount * static_cast<std::size_t>(kWritesPerMailbox));
}
