#include "executor.hpp"
#include <iostream>

void Executor::execute() { 
  while (!tasks_.empty())
  {
    const auto& task = tasks_.front();
    std::cout << "Executing task: " << task << std::endl;
    tasks_.pop();
  }
} 

void Executor::addTask(const std::string_view& task) {
  tasks_.push(std::string(task)); // Store a copy of the task string
}