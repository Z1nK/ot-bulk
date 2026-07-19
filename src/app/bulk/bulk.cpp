#include "blocking_queue.hpp"

#include <executor/executor.hpp>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <command-parser/command.hpp>
#include <command-parser/command_parser.hpp>
#include <time-utils/time.hpp>

namespace {

void runConsumer(BlockingQueue<Command>& commands) {
  CommandParser parser(3);
  Executor executor;

  auto executeIfReady = [&executor](const std::optional<std::vector<Command>>& block) {
    if (!block) {
      return;
    }
    for (const auto& command : *block) {
      executor.addTask(command);
    }
    executor.execute();
  };

  while (auto command = commands.pop()) {
    executeIfReady(parser.feedLine(*command));
  }
  executeIfReady(parser.flush());
}

}  // namespace

int main() {
  // std::cout << "Hello, World!" << argc << " " << argv[0] << std::endl;
  // std::cout << getCurrentTimeStr() << std::endl;
  // std::cout << getUnixTimestampString() << std::endl;

  BlockingQueue<Command> commands;

  std::thread consumer(runConsumer, std::ref(commands));

  // Producer: read stdin line by line, timestamp each line as it arrives,
  // and hand the resulting Command to the consumer.
  std::string line;
  while (std::getline(std::cin, line)) {
    commands.push(Command(std::move(line)));
  }
  commands.close();

  consumer.join();

  return 0;
}
