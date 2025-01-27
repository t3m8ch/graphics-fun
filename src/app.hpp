#pragma once

#include "device.hpp"
#include "gameobject.hpp"
#include "renderer.hpp"
#include "swapchain.hpp"
#include "window.hpp"

#include <vector>

#include <vulkan/vulkan_core.h>

namespace engine {

class App {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  App(SwapChain::PresentMode presentMode);
  ~App();

  App(const App &) = delete;
  App &operator=(const App &) = delete;

  void run();

private:
  void loadGameObjects();

  SwapChain::PresentMode presentMode;
  Window window{WIDTH, HEIGHT, "Hello, Vulkan"};
  Device device{window};
  Renderer renderer{window, device, presentMode};

  std::vector<GameObject> gameObjects;
};

} // namespace engine
