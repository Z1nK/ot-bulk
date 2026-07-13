#include "command_parser.hpp"

std::optional<std::string_view> CommandParser::next_line() {
  if (cursor_ >= command_line_.size()) {
    return std::nullopt;
  }

  auto newline_pos = command_line_.find('\n', cursor_);
  std::string_view line;
  if (newline_pos == std::string_view::npos) {
    line = command_line_.substr(cursor_);
    cursor_ = command_line_.size();
  } else {
    line = command_line_.substr(cursor_, newline_pos - cursor_);
    cursor_ = newline_pos + 1;
  }

  return line;
}

std::optional<std::vector<std::string_view>> CommandParser::nextBlock() {
  while (true) {
    auto line = next_line();
    if (!line) {
      if (!current_block_.empty()) {
        auto result = std::move(current_block_);
        current_block_.clear();
        return result;
      }
      return std::nullopt;
    }

    if (*line == "{") {
      ++brace_depth_;
      if (brace_depth_ == 1) {
        // entering a custom block: flush whatever static block was pending
        if (!current_block_.empty()) {
          auto result = std::move(current_block_);
          current_block_.clear();
          return result;
        }
      }
      // nested '{' markers are tracked via depth but not stored as commands
      continue;
    }

    if (*line == "}") {
      if (brace_depth_ == 0) {
        // stray closing brace outside a custom block, ignore
        continue;
      }
      --brace_depth_;
      if (brace_depth_ > 0) {
        // still inside a nested custom block, keep collecting
        continue;
      }
      auto result = std::move(current_block_);
      current_block_.clear();
      return result;
    }

    current_block_.push_back(*line);
    if (brace_depth_ == 0 && current_block_.size() == default_block_size_) {
      auto result = std::move(current_block_);
      current_block_.clear();
      return result;
    }
  }
}
