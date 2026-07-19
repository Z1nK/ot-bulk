#pragma once
#include <queue>

#include <command-parser/command.hpp>

class Executor {
public:
  void execute();

  void addTask(const Command& task);

private:
  std::queue<Command> tasks_;
};