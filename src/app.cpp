#include "app.hpp"
#include "camera.hpp"
#include "gameobject.hpp"
#include "keyboard_movement_controller.hpp"
#include "simple_render_system.hpp"

#include <chrono>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <vulkan/vulkan_core.h>

namespace engine {

App::App(SwapChain::PresentMode presentMode) : presentMode(presentMode) {
  loadGameObjects();
}

App::~App() {}

void App::run() {
  SimpleRenderSystem simpleRenderSystem{device,
                                        renderer.getSwapChainRenderPass()};
  Camera camera{};

  auto viewerObject = GameObject::create();
  KeyboardMovementController cameraController{};

  auto currentTime = std::chrono::high_resolution_clock::now();

  int fpsSamples = 60;
  float fpsSum = 0.0f;
  int frameCount = 0;

  while (!window.shouldClose()) {
    glfwPollEvents();

    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime -
                                                                   currentTime)
            .count();
    currentTime = newTime;

    cameraController.moveInPlaneXZ(window.getGLFWwindow(), frameTime,
                                   viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation,
                      viewerObject.transform.rotation);

    float aspect = renderer.getAspectRatio();
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

    if (auto commandBuffer = renderer.beginFrame()) {
      renderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
      renderer.endSwapChainRenderPass(commandBuffer);
      renderer.endFrame();
    }

    float fps = 1.0f / frameTime;
    fpsSum += fps;
    frameCount++;

    if (frameCount >= fpsSamples) {
      float averageFps = fpsSum / fpsSamples;

      std::string windowTitle =
          "Average FPS: " + std::to_string(averageFps) +
          " | Frame Time: " + std::to_string(frameTime * 1000.0f) + " ms";
      glfwSetWindowTitle(window.getGLFWwindow(), windowTitle.c_str());

      fpsSum = 0.0f;
      frameCount = 0;

      fpsSamples = static_cast<int>(averageFps);
    }
  }

  vkDeviceWaitIdle(device.device());
}

void App::loadGameObjects() {
  std::shared_ptr<Model> model =
      Model::createFromFile(device, "../models/smooth_vase.obj");

  auto gameObj = GameObject::create();
  gameObj.model = model;
  gameObj.transform.translation = {0.f, 0.f, 2.5f};
  gameObj.transform.scale = glm::vec3(3.f);

  gameObjects.push_back(std::move(gameObj));
}

} // namespace engine
