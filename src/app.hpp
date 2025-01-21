#pragma once

#include "gameobject.hpp"
#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "swapchain.hpp"

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
  void createCommandBuffers();
  void freeCommandBuffers();
  void drawFrame();
  void recreateSwapChain();
  void recordCommandBuffer(int imageIndex);
  void renderGameObjects(VkCommandBuffer commandBuffer);

  Window window{WIDTH, HEIGHT, "Hello, Vulkan"};
  Device device{window};
  std::unique_ptr<SwapChain> swapChain;
  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;
  std::vector<GameObject> gameObjects;
};

}
