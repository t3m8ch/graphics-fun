#include "simple_render_system.hpp"
#include "gameobject.hpp"
#include "pipeline.hpp"

#include <memory>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

namespace engine {

struct PushConstantData {
  glm::mat2 transform{1.f};
  glm::vec2 offset;
  alignas(16) glm::vec3 color;
};

SimpleRenderSystem::SimpleRenderSystem(Device& device, VkRenderPass renderPass) : device(device) {
  createPipelineLayout();
  createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
  vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void SimpleRenderSystem::createPipelineLayout() {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(PushConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if (
    vkCreatePipelineLayout(
      device.device(),
      &pipelineLayoutInfo,
      nullptr,
      &pipelineLayout
    ) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create pipeline layout");
  }
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
  assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  Pipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;

  pipeline = std::make_unique<Pipeline>(
    device,
    "../shaders/simple_shader.vert.spv",
    "../shaders/simple_shader.frag.spv",
    pipelineConfig
  );
}

void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects) {
  int i = 0;
  constexpr float speed = 0.00005f;

  pipeline->bind(commandBuffer);

  for (auto& obj: gameObjects) {
    i += 1;
    obj.transform2D.rotation =
        glm::mod<float>(obj.transform2D.rotation + speed * i, 2.f * glm::pi<float>());

    PushConstantData push{};
    push.offset = obj.transform2D.translation;
    push.color = obj.color;
    push.transform = obj.transform2D.mat2();

    vkCmdPushConstants(
      commandBuffer,
      pipelineLayout,
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      0,
      sizeof(PushConstantData),
      &push
    );

    obj.model->bind(commandBuffer);
    obj.model->draw(commandBuffer);
  }
}

}
