#include "executor.hpp"

#include <chrono>
#include <format>
#include <iostream>

void Executor::execute() {
  while (!tasks_.empty()) {
    const auto& task = tasks_.front();
    const auto timestamp = std::chrono::floor<std::chrono::seconds>(task.timestamp);
    std::cout << std::format("[{:%F %T}] Executing task: {}", timestamp, task.name) << std::endl;
    tasks_.pop();
  }
  std::cout << "------------------" << std::endl;
}

void Executor::addTask(const Command& task) {
  tasks_.push(task);
}