#include "executor.hpp"

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

void Executor::execute() {
  while (!tasks_.empty()) {
    const auto& batch = tasks_.front();
    if (batch.empty()) {
      tasks_.pop();
      continue;
    }

    auto first = std::chrono::time_point_cast<std::chrono::seconds>(batch.front().timestamp);

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
      tasks_.pop();
      continue;
    }

    std::cout << "bulk:";
    output_file << "bulk:";
    for (const auto& task : batch) {
      std::cout << std::format(" {}", task.name);
      output_file << std::format(" {}", task.name);
    }
    std::cout << std::endl;
    output_file << std::endl;

    tasks_.pop();
  }
}

void Executor::addTask(const std::vector<Command>& tasks) {
  tasks_.push(tasks);
}