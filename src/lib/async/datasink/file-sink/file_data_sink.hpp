#pragma once
#include <datasink/idatasink/idatasink.hpp>

class FileDataSink final : public IDataSink {
public:
  FileDataSink() = default;
  void write(std::string_view data) override;
};
