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

void runConsumer(BlockingQueue<Command>& commands, int default_block_size) {
  CommandParser parser(default_block_size);
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

int main(int argc, char* argv[]) {  
  int default_block_size;
  if (argc > 1) {
    try {
      default_block_size = std::stoi(argv[1]);
    } catch (const std::invalid_argument& e) {
      std::cerr << "Invalid argument: " << argv[1] << std::endl;
      return 1;
    }
  } else {
    default_block_size = 3;
  }

  // std::cout << getCurrentTimeStr() << std::endl;
  // std::cout << getUnixTimestampString() << std::endl;

  BlockingQueue<Command> commands;

  std::thread consumer(runConsumer, std::ref(commands), default_block_size);

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
