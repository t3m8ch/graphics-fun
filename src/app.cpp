#include "app.hpp"
#include "gameobject.hpp"
#include "pipeline.hpp"
#include "sierpinski.hpp"
#include "swapchain.hpp"

#include <array>
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

App::App() {
  loadGameObjects();
  createPipelineLayout();
  recreateSwapChain();
  createCommandBuffers();
}

App::~App() {
  vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void App::run() {
  while (!window.shouldClose()) {
    glfwPollEvents();
    drawFrame();
  }

  vkDeviceWaitIdle(device.device());
}

void App::loadGameObjects() {
  SierpinskiTriangle sierpinskiTriangle({
    {{ 0.0f, -0.5f }},
    {{ 0.5f, 0.5f }},
    {{ -0.5f, 0.5f }}
  });
  auto vertices = sierpinskiTriangle.calculate(9);
  auto model = std::make_shared<Model>(device, vertices);

  std::vector<glm::vec3> colors{
    {1.f, .7f, .73f},
    {1.f, .87f, .73f},
    {1.f, 1.f, .73f},
    {.73f, 1.f, .8f},
    {.73, .88f, 1.f}
  };
  for (auto& color : colors) {
    color = glm::pow(color, glm::vec3{2.2f});
  }
  for (int i = 0; i < 40; i++) {
    auto triangle = GameObject::create();
    triangle.model = model;
    triangle.transform2D.scale = glm::vec2(.5f) + i * 0.025f;
    triangle.transform2D.rotation = i * glm::pi<float>() * .025f;
    triangle.color = colors[i % colors.size()];
    gameObjects.push_back(std::move(triangle));
  }
}

void App::createPipelineLayout() {
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

void App::createPipeline() {
  assert(swapChain != nullptr && "Cannot create pipeline before swap chain");
  assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  Pipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = swapChain->getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;

  pipeline = std::make_unique<Pipeline>(
    device,
    "../shaders/simple_shader.vert.spv",
    "../shaders/simple_shader.frag.spv",
    pipelineConfig
  );
}

void App::recreateSwapChain() {
  auto extent = window.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = window.getExtent();
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(device.device());

  if (swapChain == nullptr) {
    swapChain = std::make_unique<SwapChain>(device, extent);
  } else {
    swapChain = std::make_unique<SwapChain>(device, extent, std::move(swapChain));
    if (swapChain->imageCount() != commandBuffers.size()) {
      freeCommandBuffers();
      createCommandBuffers();
    }
  }

  createPipeline();
}

void App::createCommandBuffers() {
  commandBuffers.resize(swapChain->imageCount());

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = device.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("Failed to allocate command buffers");
  }
}

void App::freeCommandBuffers() {
  vkFreeCommandBuffers(
    device.device(),
    device.getCommandPool(),
    static_cast<uint32_t>(commandBuffers.size()),
    commandBuffers.data()
  );
  commandBuffers.clear();
}

void App::recordCommandBuffer(int imageIndex) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("Failed to begin recording command buffer");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = swapChain->getRenderPass();
  renderPassInfo.framebuffer = swapChain->getFrameBuffer(imageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0.f;
  viewport.y = 0.f;
  viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
  viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.f;
  viewport.maxDepth = 1.f;
  VkRect2D scissor{{0, 0}, swapChain->getSwapChainExtent()};
  vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
  vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

  renderGameObjects(commandBuffers[imageIndex]);

  vkCmdEndRenderPass(commandBuffers[imageIndex]);
  if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
    throw std::runtime_error("Failed end command buffer");
  }
}

void App::renderGameObjects(VkCommandBuffer commandBuffer) {
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

void App::drawFrame() {
  uint32_t imageIndex;
  auto result = swapChain->acquireNextImage(&imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return;
  }

  if (result != VK_SUCCESS && VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire swap chain image");
  }

  recordCommandBuffer(imageIndex);
  result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized()) {
    window.resetWindowResizedFlag();
    recreateSwapChain();
    return;
  }

  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to present swap chain image");
  }
}

}
