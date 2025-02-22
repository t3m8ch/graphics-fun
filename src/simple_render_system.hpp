#pragma once

#include "camera.hpp"
#include "device.hpp"
#include "gameobject.hpp"
#include "pipeline.hpp"

#include <memory>
#include <vector>

#include <vulkan/vulkan_core.h>

namespace engine {

class SimpleRenderSystem {
public:
  SimpleRenderSystem(Device &device, VkRenderPass renderPass);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

  void renderGameObjects(VkCommandBuffer commandBuffer,
                         std::vector<GameObject> &gameObjects,
                         const Camera &camera);

private:
  void createPipelineLayout();
  void createPipeline(VkRenderPass renderPass);

  Device &device;

  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
};

} // namespace engine
