#include "executor.hpp"

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

void Executor::execute() {
  if (tasks_.empty()) {
    return;
  }

  auto first = std::chrono::time_point_cast<std::chrono::seconds>(tasks_.front().timestamp);
  // std::cout << std::format("[{}] Executing {} tasks: ", first.time_since_epoch().count(),
  // tasks_.size());

  // open filename for output, write all tasks to it, and close the file
  std::string filename = std::format("bulk{}.log", first.time_since_epoch().count());
  int counter = 1;
  while (std::filesystem::exists(filename)) {
    filename = std::format("bulk{}-{}.log", first.time_since_epoch().count(), counter);
    counter++;
  }
  std::ofstream output_file(filename);
  if (!output_file.is_open()) {
    std::cerr << "Error: could not open output file " << filename << std::endl;
    return;
  }

  std::cout << "bulk: ";
  output_file << "bulk: ";
  while (!tasks_.empty()) {
    const auto& task = tasks_.front();
    // const auto timestamp = std::chrono::floor<std::chrono::seconds>(task.timestamp);
    // std::cout << std::format("[{:%F %T}] Executing task: {}", timestamp, task.name) << std::endl;

    std::cout << std::format(" {}", task.name);
    output_file << std::format(" {}", task.name);
    tasks_.pop();
  }
  std::cout << std::endl;
  output_file << std::endl;
}

void Executor::addTask(const Command& task) {
  tasks_.push(task);
}