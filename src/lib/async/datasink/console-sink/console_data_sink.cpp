#include"console_data_sink.hpp"

void ConsoleDataSink::write(std::string_view data) {
  std::cout << data << std::endl;
}