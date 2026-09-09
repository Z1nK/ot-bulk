#include <iasync/iasync.hpp>

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  int default_block_size;
  if (argc > 1) {
    try {
      default_block_size = std::stoi(argv[1]);
    } catch (const std::invalid_argument& e) {
      std::cerr << "Invalid argument: " << argv[1] << std::endl;
      return 1;
    }
  } else {
    default_block_size = 3;
  }

  async::Context context = async::connect(default_block_size);

  std::string line;
  while (std::getline(std::cin, line)) {
    line.push_back('\n');
    async::receive(context, line.data(), line.size());
  }
  async::disconnect(context);

  return 0;
}
