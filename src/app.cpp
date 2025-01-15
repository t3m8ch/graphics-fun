#include "app.hpp"

#include <GLFW/glfw3.h>

namespace engine {

void App::run() {
  while (!window.shouldClose()) {
    glfwPollEvents();
  }
}

}
