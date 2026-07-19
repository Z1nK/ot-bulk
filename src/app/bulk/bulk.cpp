#include <iostream>
#include <string>
#include <thread>

#include <command-parser/command_parser.hpp>
#include <executor/executor.hpp>
#include <time-utils/time.hpp>

#include "blocking_queue.hpp"

namespace {

void runConsumer(BlockingQueue<std::string>& lines) {
  CommandParser parser(3);
  Executor executor;

  auto executeIfReady = [&executor](const std::optional<std::vector<std::string>>& block) {
    if (!block) {
      return;
    }
    for (const auto& command : *block) {
      executor.addTask(command);
    }
    executor.execute();
  };

  while (auto line = lines.pop()) {
    executeIfReady(parser.feedLine(*line));
  }
  executeIfReady(parser.flush());
}

}  // namespace

int main(int argc, char* argv[]) {
  std::cout << "Hello, World!" << argc << " " << argv[0] << std::endl;
  std::cout << getCurrentTimeStr() << std::endl;
  std::cout << getUnixTimestampString() << std::endl;

  BlockingQueue<std::string> lines;

  std::thread consumer(runConsumer, std::ref(lines));

  // Producer: read stdin line by line and hand each line to the consumer.
  std::string line;
  while (std::getline(std::cin, line)) {
    lines.push(std::move(line));
  }
  lines.close();

  consumer.join();

  return 0;
}
