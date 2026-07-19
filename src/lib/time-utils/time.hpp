#pragma once
#include <chrono>
#include <format>
#include <string>

inline void getCurrentTimeStr(std::string* time_str) {
  using namespace std::chrono;
  const auto now = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
  const auto zoned = std::chrono::zoned_time{std::chrono::current_zone(), now};
  time_str->clear();
  std::format_to(std::back_inserter(*time_str), "{:%c}", zoned);
}

inline std::string getCurrentTimeStr() {
  std::string time_str;
  getCurrentTimeStr(&time_str);
  return time_str;
}

inline std::string getUnixTimestampString() {
  auto now = std::chrono::system_clock::now();
  auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
  auto value = seconds.time_since_epoch().count();

  return std::format("{}", value);
}