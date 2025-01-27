#include "window.hpp"

#include <GLFW/glfw3.h>
#include <stdexcept>

namespace engine {

Window::Window(int width, int height, std::string name)
    : width(width), height(height), name(name) {
  initWindow();
}

Window::~Window() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

bool Window::shouldClose() { return glfwWindowShouldClose(window); }

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
  if (glfwCreateWindowSurface(instance, window, nullptr, surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create window surface");
  }
}

void Window::framebufferResizeCallback(GLFWwindow *window, int width,
                                       int height) {
  auto engineWindow =
      reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
  engineWindow->framebufferResized = true;
  engineWindow->width = width;
  engineWindow->height = height;
}

void Window::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

} // namespace engine
