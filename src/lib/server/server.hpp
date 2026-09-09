#pragma once
#include <boost/asio.hpp>

#include <cstdint>

class TcpServer final {
public:
  struct Settings {
    std::uint16_t port;
  };

  explicit TcpServer(boost::asio::io_context& io, Settings settings);

  void run();

private:
  boost::asio::awaitable<void> acceptLoop();
  static boost::asio::awaitable<void> handleSession(boost::asio::ip::tcp::socket socket);

  boost::asio::io_context& io_;
  Settings settings_;
};
