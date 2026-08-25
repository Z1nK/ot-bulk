#pragma once
#include <datasink/idatasink/idatasink.hpp>

#include <fstream>

class FileDataSink final : public IDataSink {
private:
  std::ofstream file_;

public:
  explicit FileDataSink(const std::string& filename) : file_(filename) {}

  void write(std::string_view data) override {
    if (file_.is_open()) {
      file_ << data << "\n";
    }
  }

  void flush() override { file_.flush(); }
};