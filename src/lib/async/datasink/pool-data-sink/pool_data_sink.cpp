#include "pool_data_sink.hpp"

#include <string>
#include <utility>

PoolDataSink::PoolDataSink(std::vector<std::shared_ptr<Mailbox>> mailboxes)
    : mailboxes_(std::move(mailboxes)) {}

void PoolDataSink::write(std::string_view data) {
  std::size_t idx = next_++ % mailboxes_.size();
  mailboxes_[idx]->post(Message{std::string(data), nullptr});
}

void PoolDataSink::flush() {
  for (auto& mailbox : mailboxes_) {
    mailbox->drain();
  }
}
