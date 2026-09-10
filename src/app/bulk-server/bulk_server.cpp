#include <iasync/iasync.hpp>
#include <server/server.hpp>

#include <boost/asio.hpp>

#include <iostream>
#include <string>

using boost::asio::awaitable;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

namespace {

awaitable<void> handleSession(tcp::socket socket, std::size_t block_size) {
  async::Context context = async::connect(block_size);

  boost::asio::streambuf buffer;
  try {
    for (;;) {
      co_await boost::asio::async_read_until(socket, buffer, "\n", use_awaitable);
      std::istream is(&buffer);
      std::string line;
      std::getline(is, line);
      line.push_back('\n');
      async::receive(context, line.data(), line.size());
    }
  } catch (const boost::system::system_error& e) {
    if (e.code() != boost::asio::error::eof) {
      std::cerr << "[bulk-server] session error: " << e.what() << '\n';
    }
  }

  async::disconnect(context);
}

}  // namespace

int main(int argc, char* argv[]) {
  std::uint16_t port = 9000;
  std::size_t block_size = 3;

  try {
    if (argc > 1) {
      port = static_cast<std::uint16_t>(std::stoi(argv[1]));
    }
    if (argc > 2) {
      block_size = static_cast<std::size_t>(std::stoul(argv[2]));
    }
  } catch (const std::exception& e) {
    std::cerr << "Invalid argument: " << e.what() << '\n';
    return 1;
  }

  boost::asio::io_context io;
  TcpServer server(io, {.port = port, .on_session = [block_size](tcp::socket socket) {
                          return handleSession(std::move(socket), block_size);
                        }});
  server.run();
  io.run();

  return 0;
}
