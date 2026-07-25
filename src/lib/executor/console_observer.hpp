#pragma once

#include "observer.hpp"

#include <iostream>
#include <format>

class ConsoleObserver : public Observer {
public:
  void onBlockUpdate(const std::vector<Command>& block) override {
    std::cout << "bulk:";
    for (const auto& command : block) {
      std::cout << std::format(" {}", command.name);
    }
    std::cout << std::endl;
  }
};