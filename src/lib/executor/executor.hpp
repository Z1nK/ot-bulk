#pragma once
#include <iostream>
#include <queue>
#include <string>

class Executor {
public:
  void execute();

  void addTask(const std::string_view& task);

private:
  std::queue<std::string> tasks_;
};