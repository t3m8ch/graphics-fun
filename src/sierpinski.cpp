#include "sierpinski.hpp"

#include <array>
#include <cmath>

namespace engine {

SierpinskiTriangle::SierpinskiTriangle(
    const std::vector<Model::Vertex> &triangle) {
  this->triangle.p0 = {triangle[0].position.x, triangle[0].position.y};
  this->triangle.p1 = {triangle[1].position.x, triangle[1].position.y};
  this->triangle.p2 = {triangle[2].position.x, triangle[2].position.y};
}

std::vector<Model::Vertex> SierpinskiTriangle::calculate(uint32_t n) {
  uint32_t trianglesCount = caclulateTrianglesCount(n);

  std::vector<Triangle> triangles1;
  triangles1.reserve(trianglesCount);

  std::vector<Triangle> triangles2;
  triangles2.reserve(trianglesCount);

  triangles1.push_back(triangle);
  f(triangles1, triangles2, n);

  return mapTrianglesToVertices(triangles2.empty() ? triangles1 : triangles2);
}

void SierpinskiTriangle::f(
    std::vector<SierpinskiTriangle::Triangle> &triangles1,
    std::vector<SierpinskiTriangle::Triangle> &triangles2, uint32_t n) {
  if (n == 0) {
    return;
  }

  for (auto t : triangles1) {
    trip(t, triangles2);
  }
  triangles1.clear();

  f(triangles2, triangles1, n - 1);
}

SierpinskiTriangle::Point
SierpinskiTriangle::mid(SierpinskiTriangle::Point p0,
                        SierpinskiTriangle::Point p1) {
  return {(p0.x + p1.x) / 2.f, (p0.y + p1.y) / 2.f};
}

void SierpinskiTriangle::trip(
    SierpinskiTriangle::Triangle t,
    std::vector<SierpinskiTriangle::Triangle> &triangles) {
  auto mid_t0_t1 = mid(t.p0, t.p1);
  auto mid_t0_t2 = mid(t.p0, t.p2);
  auto mid_t1_t2 = mid(t.p1, t.p2);

  triangles.push_back({t.p0, mid_t0_t1, mid_t0_t2});
  triangles.push_back({mid_t0_t1, t.p1, mid_t1_t2});
  triangles.push_back({mid_t0_t2, mid_t1_t2, t.p2});
}

uint32_t SierpinskiTriangle::caclulateTrianglesCount(uint32_t n) {
  return pow(3, n);
}

std::vector<Model::Vertex>
SierpinskiTriangle::mapTrianglesToVertices(std::vector<Triangle> &triangles) {
  std::vector<Model::Vertex> vertices;
  vertices.reserve(triangles.size() * 3);

  for (auto t : triangles) {
    std::array<Model::Vertex, 3> triangleVertices{};

    triangleVertices[0].position = glm::vec2(t.p0.x, t.p0.y);
    triangleVertices[0].color = glm::vec3(1.f, 0.f, 0.f);

    triangleVertices[1].position = glm::vec2(t.p1.x, t.p1.y);
    triangleVertices[1].color = glm::vec3(0.f, 1.f, 0.f);

    triangleVertices[2].position = glm::vec2(t.p2.x, t.p2.y);
    triangleVertices[2].color = glm::vec3(0.f, 0.f, 1.f);

    for (auto vertex : triangleVertices) {
      vertices.push_back(vertex);
    }
  }

  return vertices;
}

} // namespace engine
