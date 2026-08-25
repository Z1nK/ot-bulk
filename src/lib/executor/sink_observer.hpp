#pragma once

#include "observer.hpp"

#include <datasink/idatasink/idatasink.hpp>

#include <format>
#include <memory>
#include <string>

class SinkObserver : public Observer {
public:
  explicit SinkObserver(std::shared_ptr<IDataSink> sink) : sink_(std::move(sink)) {}

  void onBlockUpdate(const std::vector<Command>& block) override {
    if (block.empty()) {
      return;
    }

    std::string line = "bulk:";
    for (const auto& command : block) {
      line += std::format(" {}", command.name);
    }
    sink_->write(line);
  }

private:
  std::shared_ptr<IDataSink> sink_;
};
