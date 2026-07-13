#pragma once
#include <string_view>
#include <optional>
#include <string>
#include <vector>

class CommandParser {
public:
  CommandParser(std::string_view command_line, size_t default_block_size)
      : command_line_(command_line), default_block_size_(default_block_size), cursor_(0) {};

  std::optional<std::vector<std::string_view>> nextBlock(); 

private:
  std::string_view command_line_;
  size_t default_block_size_;
  size_t cursor_;

  std::vector<std::string_view> current_block_;
  size_t brace_depth_ = 0;

  // for internal use, to get the next line from the command line (until \n)
  std::optional<std::string_view> next_line();
};
