#include "app.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main() {
  engine::SwapChain::PresentMode presentMode = engine::SwapChain::FIFO;
  const char *pPresentModeChars = std::getenv("PRESENT_MODE");

  if (pPresentModeChars != nullptr) {
    std::string presentModeStr = pPresentModeChars;
    if (presentModeStr == "FIFO") {
      presentMode = engine::SwapChain::FIFO;
    } else if (presentModeStr == "MAILBOX") {
      presentMode = engine::SwapChain::MAILBOX;
    } else if (presentModeStr == "IMMEDIATE") {
      presentMode = engine::SwapChain::IMMEDIATE;
    }
  }

  engine::App app{presentMode};

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
