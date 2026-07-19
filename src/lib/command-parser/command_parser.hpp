#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class CommandParser {
public:
  explicit CommandParser(size_t default_block_size) : default_block_size_(default_block_size) {};

  // Feed a single line of input. Returns a completed block if this line
  // completed one (static block reached its size, or a custom '{'/'}' block
  // closed), std::nullopt otherwise.
  std::optional<std::vector<std::string>> feedLine(std::string_view line);

  // Call once the input stream is exhausted to retrieve any trailing
  // partial static block that never reached default_block_size_.
  std::optional<std::vector<std::string>> flush();

private:
  size_t default_block_size_;

  std::vector<std::string> current_block_;
  size_t brace_depth_ = 0;
};
