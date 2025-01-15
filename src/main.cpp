#include "app.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main() {
  engine::App app{};

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
