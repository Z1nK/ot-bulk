#pragma once
#include "observer.hpp"

#include <memory>
#include <queue>
#include <vector>

#include <command-parser/command.hpp>

class Executor {
public:
  void execute();
  void subscribe(std::shared_ptr<Observer> observer);

  void addTask(const std::vector<Command>& tasks);

private:
  std::queue<std::vector<Command>> tasks_;
  std::vector<std::shared_ptr<Observer>> observers_;
};