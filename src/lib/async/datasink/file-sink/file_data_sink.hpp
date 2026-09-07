#pragma once
#include <datasink/idatasink/idatasink.hpp>

#include <cstddef>
#include <optional>

class FileDataSink final : public IDataSink {
public:
  explicit FileDataSink(std::optional<std::size_t> worker_index = std::nullopt);
  void write(std::string_view data) override;

private:
  std::optional<std::size_t> worker_index_;
};
