#include <executor/executor.hpp>

#include <memory>
#include <vector>

#include <gtest/gtest.h>

namespace {

class FakeObserver : public Observer {
public:
  void onBlockUpdate(const std::vector<Command>& block) override {
    blocks.push_back(block);
  }

  std::vector<std::vector<Command>> blocks;
};

}  // namespace

TEST(ExecutorTest, ExecuteWithNoTasksNotifiesNoObservers) {
  Executor executor;
  auto observer = std::make_shared<FakeObserver>();
  executor.subscribe(observer);

  executor.execute();

  EXPECT_TRUE(observer->blocks.empty());
}

TEST(ExecutorTest, ExecuteNotifiesAllObserversWithEachBlockAndDrainsQueue) {
  Executor executor;
  auto first_observer = std::make_shared<FakeObserver>();
  auto second_observer = std::make_shared<FakeObserver>();
  executor.subscribe(first_observer);
  executor.subscribe(second_observer);

  std::vector<Command> block1{Command("cmd1"), Command("cmd2")};
  std::vector<Command> block2{Command("cmd3")};
  executor.addTask(block1);
  executor.addTask(block2);

  executor.execute();

  for (const auto* observer : {first_observer.get(), second_observer.get()}) {
    ASSERT_EQ(observer->blocks.size(), 2u);
    EXPECT_EQ(observer->blocks[0].size(), 2u);
    EXPECT_EQ(observer->blocks[0][0].name, "cmd1");
    EXPECT_EQ(observer->blocks[0][1].name, "cmd2");
    EXPECT_EQ(observer->blocks[1].size(), 1u);
    EXPECT_EQ(observer->blocks[1][0].name, "cmd3");
  }

  // A second execute() on the now-empty queue must not notify observers again.
  executor.execute();
  EXPECT_EQ(first_observer->blocks.size(), 2u);
  EXPECT_EQ(second_observer->blocks.size(), 2u);
}
