#include "app.hpp"
#include "gameobject.hpp"
#include "pipeline.hpp"
#include "sierpinski.hpp"

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
  createPipeline();
}

App::~App() {
  vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void App::run() {
  while (!window.shouldClose()) {
    glfwPollEvents();

    if (auto commandBuffer = renderer.beginFrame()) {
      renderer.beginSwapChainRenderPass(commandBuffer);
      renderGameObjects(commandBuffer);
      renderer.endSwapChainRenderPass(commandBuffer);
      renderer.endFrame();
    }
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
  assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  Pipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = renderer.getSwapChainRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;

  pipeline = std::make_unique<Pipeline>(
    device,
    "../shaders/simple_shader.vert.spv",
    "../shaders/simple_shader.frag.spv",
    pipelineConfig
  );
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

}
