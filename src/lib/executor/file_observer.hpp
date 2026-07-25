#pragma once

#include "observer.hpp"

#include <format>
#include <iostream>
#include <filesystem>
#include <fstream>

class FileObserver : public Observer {
public:
  void onBlockUpdate(const std::vector<Command>& block) override {
    if (block.empty()) {
      return;
    }

    auto first = std::chrono::time_point_cast<std::chrono::seconds>(block.front().timestamp);

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

    output_file << "bulk:";
    for (const auto& command : block) {
      output_file << std::format(" {}", command.name);
    }
    output_file << std::endl;
  }
};