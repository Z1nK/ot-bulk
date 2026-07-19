#include <executor/executor.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

namespace fs = std::filesystem;

namespace {

class ExecutorTest : public ::testing::Test {
protected:
  void SetUp() override {
    original_cwd_ = fs::current_path();
    test_dir_ = fs::temp_directory_path() /
                ("executor_test_" +
                 std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directories(test_dir_);
    fs::current_path(test_dir_);
  }

  void TearDown() override {
    fs::current_path(original_cwd_);
    fs::remove_all(test_dir_);
  }

  size_t countEntries() const {
    return static_cast<size_t>(
        std::distance(fs::directory_iterator(test_dir_), fs::directory_iterator{}));
  }

  // Returns the single *.log file written to the current directory, or
  // std::nullopt if none/more than one exists.
  std::optional<fs::path> findLogFile() const {
    std::optional<fs::path> found;
    for (const auto& entry : fs::directory_iterator(test_dir_)) {
      if (entry.path().extension() == ".log") {
        if (found) {
          return std::nullopt;  // ambiguous, more than one
        }
        found = entry.path();
      }
    }
    return found;
  }

  fs::path original_cwd_;
  fs::path test_dir_;
};

}  // namespace

TEST_F(ExecutorTest, ExecuteWithNoTasksWritesNoLogFile) {
  Executor executor;
  executor.execute();

  EXPECT_EQ(countEntries(), 0u);
}

TEST_F(ExecutorTest, ExecuteWritesAllTasksToLogFileAndDrainsQueue) {
  Executor executor;
  executor.addTask(Command("cmd1"));
  executor.addTask(Command("cmd2"));
  executor.addTask(Command("cmd3"));

  executor.execute();

  auto log_file = findLogFile();
  ASSERT_TRUE(log_file);

  std::ifstream in(*log_file);
  std::ostringstream contents;
  contents << in.rdbuf();

  EXPECT_EQ(contents.str(), "bulk:  cmd1 cmd2 cmd3\n");

  // A second execute() on the now-empty queue must not write another file.
  executor.execute();
  EXPECT_EQ(countEntries(), 1u);
}
