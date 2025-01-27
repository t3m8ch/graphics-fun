#pragma once

#include "model.hpp"

#include <vector>

namespace engine {

class SierpinskiTriangle {
public:
  SierpinskiTriangle(const std::vector<Model::Vertex> &triangle);
  std::vector<Model::Vertex> calculate(uint32_t n);

  struct Point {
    float x, y;
  };

  struct Triangle {
    Point p0, p1, p2;
  };

private:
  Triangle triangle;

  void f(std::vector<Triangle> &triangles1, std::vector<Triangle> &triangles2,
         uint32_t n);
  Point mid(Point p0, Point p1);
  void trip(Triangle t, std::vector<Triangle> &triangles);
  uint32_t caclulateTrianglesCount(uint32_t n);
  std::vector<Model::Vertex>
  mapTrianglesToVertices(std::vector<Triangle> &triangles);
};

} // namespace engine
