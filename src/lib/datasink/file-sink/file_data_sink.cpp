#include"file_data_sink.hpp"

void FileDataSink::write(std::string_view data) {
  if (file_.is_open()) {
    file_ << data << "\n";
  }
}