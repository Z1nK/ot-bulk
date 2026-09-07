#include <datasink/pool-data-sink/pool_data_sink.hpp>

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {

// Records writes into storage owned by the test, not by the sink itself, so
// the recorded data stays valid even after PoolDataSink (and the FakeDataSink
// instances it owns) has been destroyed.
class FakeDataSink : public IDataSink {
public:
  explicit FakeDataSink(std::vector<std::string>* out) : out_(out) {}

  void write(std::string_view data) override { out_->emplace_back(data); }

private:
  std::vector<std::string>* out_;
};

}  // namespace

TEST(PoolDataSinkTest, FactoryIsCalledOncePerWorkerWithItsIndex) {
  std::vector<std::size_t> indices;
  std::vector<std::string> discard;

  PoolDataSink sink(
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

TEST(PoolDataSinkTest, WriteRoundRobinsAcrossWorkersInOrder) {
  std::vector<std::vector<std::string>> recorded(2);

  PoolDataSink sink(
      [&recorded](std::size_t worker_index) { return std::make_unique<FakeDataSink>(&recorded[worker_index]); },
      2);

  sink.write("block0");
  sink.write("block1");
  sink.write("block2");
  sink.write("block3");
  sink.write("block4");

  sink.flush();

  ASSERT_EQ(recorded[0].size(), 3u);
  EXPECT_EQ(recorded[0][0], "block0");
  EXPECT_EQ(recorded[0][1], "block2");
  EXPECT_EQ(recorded[0][2], "block4");

  ASSERT_EQ(recorded[1].size(), 2u);
  EXPECT_EQ(recorded[1][0], "block1");
  EXPECT_EQ(recorded[1][1], "block3");
}

TEST(PoolDataSinkTest, DestructorDrainsPendingWrites) {
  std::vector<std::vector<std::string>> recorded(2);

  {
    PoolDataSink sink(
        [&recorded](std::size_t worker_index) { return std::make_unique<FakeDataSink>(&recorded[worker_index]); },
        2);

    sink.write("a");
    sink.write("b");
    sink.write("c");
  }

  EXPECT_EQ(recorded[0].size(), 2u);
  EXPECT_EQ(recorded[1].size(), 1u);
}
