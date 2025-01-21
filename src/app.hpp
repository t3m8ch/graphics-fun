#pragma once

#include "gameobject.hpp"
#include "window.hpp"
#include "device.hpp"
#include "renderer.hpp"

#include <vector>

#include <vulkan/vulkan_core.h>

namespace engine {

class App {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  App();
  ~App();

  App(const App&) = delete;
  App& operator=(const App&) = delete;

  void run();
private:
  void loadGameObjects();

  Window window{WIDTH, HEIGHT, "Hello, Vulkan"};
  Device device{window};
  Renderer renderer{window, device};

  std::vector<GameObject> gameObjects;
};

}
