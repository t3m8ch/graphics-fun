#pragma once

#include "gameobject.hpp"
#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "renderer.hpp"

#include <memory>
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
  void createPipelineLayout();
  void createPipeline();
  void renderGameObjects(VkCommandBuffer commandBuffer);

  Window window{WIDTH, HEIGHT, "Hello, Vulkan"};
  Device device{window};
  Renderer renderer{window, device};

  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<GameObject> gameObjects;
};

}
