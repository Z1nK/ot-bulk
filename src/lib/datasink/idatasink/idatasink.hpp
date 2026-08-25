#pragma once
#include <string_view>

class IDataSink {
public:
  virtual ~IDataSink() = default;

  virtual void write(std::string_view data) = 0;
  virtual void flush() {}  
};