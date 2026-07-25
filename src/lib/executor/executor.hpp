#pragma once
#include <queue>
#include <vector>

#include <command-parser/command.hpp>

class Executor {
public:
  void execute();

  void addTask(const std::vector<Command>& tasks);

private:
  std::queue<std::vector<Command>> tasks_;
};