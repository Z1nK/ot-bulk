#pragma once
#include "command.hpp"

#include <optional>
#include <string_view>
#include <vector>

class CommandParser {
public:
  explicit CommandParser(size_t default_block_size) : default_block_size_(default_block_size) {};

  // Feed a single command (already timestamped on arrival). Returns a
  // completed block if this command completed one (static block reached its
  // size, or a custom '{'/'}' block closed), std::nullopt otherwise.
  std::optional<std::vector<Command>> feedLine(const Command& command);

  // Call once the input stream is exhausted to retrieve any trailing
  // partial static block that never reached default_block_size_.
  std::optional<std::vector<Command>> flush();

private:
  size_t default_block_size_;

  std::vector<Command> current_block_;
  size_t brace_depth_ = 0;
};
