#pragma once

#include "model.hpp"

#include <cstdint>
#include <memory>

#include <glm/gtc/matrix_transform.hpp>

namespace engine {

struct TransformComponent {
  glm::vec3 translation{};
  glm::vec3 scale{1.f, 1.f, 1.f};
  glm::vec3 rotation{};

  glm::mat4 mat4();
  glm::mat3 normalMatrix();
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
  TransformComponent transform{};

private:
  GameObject(id_t id) : id(id) {}

  id_t id;
};

} // namespace engine
