#pragma once
#include <boost/asio.hpp>

#include <cstdint>
#include <functional>

class TcpServer final {
public:
  using SessionHandler = std::function<boost::asio::awaitable<void>(boost::asio::ip::tcp::socket)>;

  struct Settings {
    std::uint16_t port;
    SessionHandler on_session;
  };

  explicit TcpServer(boost::asio::io_context& io, Settings settings);

  void run();

private:
  boost::asio::awaitable<void> acceptLoop();

  boost::asio::io_context& io_;
  Settings settings_;
};
