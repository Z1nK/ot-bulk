#include "executor.hpp"


void Executor::execute() {
  while (!tasks_.empty()) {
    const auto& block = tasks_.front();
    if (!block.empty()) {
      for (const auto& observer : observers_) {
        observer->onBlockUpdate(block);
      }
    }
    tasks_.pop();
  }
}

void Executor::subscribe(std::shared_ptr<Observer> observer) {
  observers_.push_back(observer);
}

void Executor::addTask(const std::vector<Command>& tasks) {
  tasks_.push(tasks);
}