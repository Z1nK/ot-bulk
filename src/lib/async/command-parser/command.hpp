#pragma once
#include <chrono>
#include <string>

struct Command {
  std::chrono::system_clock::time_point timestamp;
  std::string name;

  Command(std::string n,
          const std::chrono::system_clock::time_point ts = std::chrono::system_clock::now())
      : timestamp(ts), name(std::move(n)) {}
};