#include "app.hpp"
#include "gameobject.hpp"
#include "sierpinski.hpp"
#include "simple_render_system.hpp"

#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vulkan/vulkan_core.h>

namespace engine {

App::App() { loadGameObjects(); }

App::~App() {}

void App::run() {
  SimpleRenderSystem simpleRenderSystem{device,
                                        renderer.getSwapChainRenderPass()};

  while (!window.shouldClose()) {
    glfwPollEvents();

    if (auto commandBuffer = renderer.beginFrame()) {
      renderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
      renderer.endSwapChainRenderPass(commandBuffer);
      renderer.endFrame();
    }
  }

  vkDeviceWaitIdle(device.device());
}

void App::loadGameObjects() {
  SierpinskiTriangle sierpinskiTriangle(
      {{{0.0f, -0.5f}}, {{0.5f, 0.5f}}, {{-0.5f, 0.5f}}});
  auto vertices = sierpinskiTriangle.calculate(9);
  auto model = std::make_shared<Model>(device, vertices);

  std::vector<glm::vec3> colors{{1.f, .7f, .73f},
                                {1.f, .87f, .73f},
                                {1.f, 1.f, .73f},
                                {.73f, 1.f, .8f},
                                {.73, .88f, 1.f}};
  for (auto &color : colors) {
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

} // namespace engine
