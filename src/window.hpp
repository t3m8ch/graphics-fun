#pragma once

#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace engine {

class Window {
public:
  Window(int width, int height, std::string name);
  ~Window();

  Window(const Window&) = delete;
  Window &operator=(const Window&) = delete;

  bool shouldClose();
  VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
  bool wasWindowResized() { return framebufferResized; }
  void resetWindowResizedFlag() { framebufferResized = false; }

  void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

private:
  static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
  void initWindow();

  int width;
  int height;
  bool framebufferResized = false;

  std::string name;
  GLFWwindow* window;
};

}
