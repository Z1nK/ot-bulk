#pragma once

#include <string>
#include <vector>

#include <command-parser/command.hpp>

class Observer {
public:  
  virtual void onBlockUpdate(const std::vector<Command>& block) = 0;
  virtual ~Observer() = default;
};