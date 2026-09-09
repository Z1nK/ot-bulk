#include "server.hpp"

#include <iostream>
#include <string>

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

TcpServer::TcpServer(boost::asio::io_context& io, Settings settings) : io_(io), settings_(settings) {}

void TcpServer::run() {
  co_spawn(io_, acceptLoop(), detached);
}

awaitable<void> TcpServer::acceptLoop() {
  auto executor = co_await boost::asio::this_coro::executor;
  tcp::acceptor acceptor(executor, tcp::endpoint(tcp::v4(), settings_.port));

  for (;;) {
    tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
    co_spawn(executor, handleSession(std::move(socket)), detached);
  }
}

awaitable<void> TcpServer::handleSession(tcp::socket socket) {
  boost::asio::streambuf buffer;
  co_await boost::asio::async_read_until(socket, buffer, "\n", use_awaitable);

  std::istream is(&buffer);
  std::string line;
  std::getline(is, line);
  std::cout << "[server] received: \"" << line << "\"\n";

  std::string reply = line + "\n";
  co_await boost::asio::async_write(socket, boost::asio::buffer(reply), use_awaitable);
}
