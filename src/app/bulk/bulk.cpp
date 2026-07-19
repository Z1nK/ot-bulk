

#include <iostream>
#include <sstream>
#include <string>

#include <command-parser/command_parser.hpp>
#include <executor/executor.hpp>
#include <time-utils/time.hpp>

int main(int argc, char* argv[]) {
  std::cout << "Hello, World!" << argc << " " << argv[0] << std::endl;

  std::ostringstream input_buffer;
  input_buffer << std::cin.rdbuf();
  std::string input_data = input_buffer.str();

  CommandParser parser(input_data, 3);
  Executor executor;

  std::cout << getCurrentTimeStr() << std::endl;
  std::cout << getUnixTimestampString() << std::endl;

  for (auto block = parser.nextBlock(); block; block = parser.nextBlock()) {
    std::cout << "Block: ";
    for (const auto& command : *block) {
      std::cout << command << " ";
      executor.addTask(command);
    }
    std::cout << std::endl;
    executor.execute();
  }

  return 0;
}