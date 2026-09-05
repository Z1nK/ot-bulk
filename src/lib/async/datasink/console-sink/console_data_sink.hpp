#pragma once
#include <datasink/idatasink/idatasink.hpp>
#include <iostream>

class ConsoleDataSink final : public IDataSink {
public:
  ConsoleDataSink() = default;
  void write(std::string_view data) override;
};