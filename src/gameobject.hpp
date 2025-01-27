#pragma once

#include "model.hpp"

#include <cstdint>
#include <memory>

namespace engine {

struct Transform2DComponent {
  glm::vec2 translation{};
  glm::vec2 scale{1.f, 1.f};
  float rotation;

  glm::mat2 mat2() {
    const float s = glm::sin(rotation);
    const float c = glm::cos(rotation);

    glm::mat2 rotationMat{{c, s}, {-s, c}};

    glm::mat2 scaleMat{{scale.x, 0.f}, {0.f, scale.y}};

    return rotationMat * scaleMat;
  }
};

class GameObject {
public:
  using id_t = uint32_t;

  static GameObject create() {
    static id_t currentId = 0;
    return GameObject{currentId++};
  }

  GameObject(const GameObject &) = delete;
  GameObject(GameObject &&) = default;
  GameObject &operator=(const GameObject &) = delete;
  GameObject &operator=(GameObject &&) = default;

  id_t getId() const { return id; }

  std::shared_ptr<Model> model{};
  glm::vec3 color{};
  Transform2DComponent transform2D{};

private:
  GameObject(id_t id) : id(id) {}

  id_t id;
};

} // namespace engine
