#include "command_parser.hpp"

std::optional<std::vector<Command>> CommandParser::feedLine(const Command& command) {
  const std::string_view line = command.name;

  if (line == "{") {
    ++brace_depth_;
    if (brace_depth_ == 1 && !current_block_.empty()) {
      // entering a custom block: flush whatever static block was pending
      auto result = std::move(current_block_);
      current_block_.clear();
      return result;
    }
    // nested '{' markers are tracked via depth but not stored as commands
    return std::nullopt;
  }

  if (line == "}") {
    if (brace_depth_ == 0) {
      // stray closing brace outside a custom block, ignore
      return std::nullopt;
    }
    --brace_depth_;
    if (brace_depth_ > 0) {
      // still inside a nested custom block, keep collecting
      return std::nullopt;
    }
    auto result = std::move(current_block_);
    current_block_.clear();
    return result;
  }

  current_block_.push_back(command);
  if (brace_depth_ == 0 && current_block_.size() == default_block_size_) {
    auto result = std::move(current_block_);
    current_block_.clear();
    return result;
  }

  return std::nullopt;
}

std::optional<std::vector<Command>> CommandParser::flush() {
  if (brace_depth_ > 0) {
    current_block_.clear();
    brace_depth_ = 0;
    return std::nullopt;
  }

  if (current_block_.empty()) {
    return std::nullopt;
  }
  auto result = std::move(current_block_);
  current_block_.clear();
  return result;
}
