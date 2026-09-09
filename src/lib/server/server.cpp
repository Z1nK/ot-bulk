#include "server.hpp"

#include <utility>

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

TcpServer::TcpServer(boost::asio::io_context& io, Settings settings) : io_(io), settings_(std::move(settings)) {}

void TcpServer::run() {
  co_spawn(io_, acceptLoop(), detached);
}

awaitable<void> TcpServer::acceptLoop() {
  auto executor = co_await boost::asio::this_coro::executor;
  tcp::acceptor acceptor(executor, tcp::endpoint(tcp::v4(), settings_.port));

  for (;;) {
    tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
    co_spawn(executor, settings_.on_session(std::move(socket)), detached);
  }
}
