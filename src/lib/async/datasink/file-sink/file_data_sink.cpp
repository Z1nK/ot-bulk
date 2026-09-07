#include "file_data_sink.hpp"

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

FileDataSink::FileDataSink(std::optional<std::size_t> worker_index) : worker_index_(worker_index) {}

void FileDataSink::write(std::string_view data) {
  auto now = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());

  std::string base = worker_index_
                          ? std::format("bulk{}-w{}", now.time_since_epoch().count(), *worker_index_)
                          : std::format("bulk{}", now.time_since_epoch().count());

  std::string filename = base + ".log";
  int counter = 1;
  while (std::filesystem::exists(filename)) {
    filename = std::format("{}-{}.log", base, counter);
    counter++;
  }

  std::ofstream output_file(filename);
  if (!output_file.is_open()) {
    std::cerr << "Error: could not open output file " << filename << std::endl;
    return;
  }

  output_file << data << std::endl;
}
