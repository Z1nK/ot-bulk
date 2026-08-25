#include "file_data_sink.hpp"

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

void FileDataSink::write(std::string_view data) {
  auto now = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());

  std::string filename = std::format("bulk{}.log", now.time_since_epoch().count());
  int counter = 1;
  while (std::filesystem::exists(filename)) {
    filename = std::format("bulk{}-{}.log", now.time_since_epoch().count(), counter);
    counter++;
  }

  std::ofstream output_file(filename);
  if (!output_file.is_open()) {
    std::cerr << "Error: could not open output file " << filename << std::endl;
    return;
  }

  output_file << data << std::endl;
}
