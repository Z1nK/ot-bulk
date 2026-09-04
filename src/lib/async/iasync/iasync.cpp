#include "iasync.hpp"

#include <executor/executor.hpp>
#include <executor/sink_observer.hpp>

#include <memory>
#include <string>
#include <string_view>

#include <command-parser/command.hpp>
#include <command-parser/command_parser.hpp>
#include <datasink/async-data-sink/async_data_sink.hpp>
#include <datasink/console-sink/console_data_sink.hpp>
#include <datasink/file-sink/file_data_sink.hpp>

namespace async {

namespace {

struct Connection {
  explicit Connection(std::size_t bulk_size) : parser(bulk_size) {
    executor.subscribe(std::make_shared<SinkObserver>(
        std::make_unique<AsyncDataSink>(std::make_unique<ConsoleDataSink>())));
    executor.subscribe(std::make_shared<SinkObserver>(
        std::make_unique<AsyncDataSink>(std::make_unique<FileDataSink>())));
  }

  CommandParser parser;
  Executor executor;
  std::string pending;
};

void dispatchIfReady(Connection& connection, const std::optional<std::vector<Command>>& block) {
  if (!block) {
    return;
  }
  connection.executor.addTask(*block);
  connection.executor.execute();
}

void feedLine(Connection& connection, std::string_view line) {
  dispatchIfReady(connection, connection.parser.feedLine(Command(std::string(line))));
}

}  // namespace

Context connect(std::size_t bulk_size) {
  return new Connection(bulk_size);
}

void receive(Context context, const char* data, std::size_t size) {
  auto* connection = static_cast<Connection*>(context);
  connection->pending.append(data, size);

  std::size_t start = 0;
  while (true) {
    const auto pos = connection->pending.find('\n', start);
    if (pos == std::string::npos) {
      break;
    }
    feedLine(*connection, std::string_view(connection->pending).substr(start, pos - start));
    start = pos + 1;
  }
  connection->pending.erase(0, start);
}

void disconnect(Context context) {
  auto* connection = static_cast<Connection*>(context);

  if (!connection->pending.empty()) {
    feedLine(*connection, connection->pending);
    connection->pending.clear();
  }

  dispatchIfReady(*connection, connection->parser.flush());

  delete connection;
}

}  // namespace async
