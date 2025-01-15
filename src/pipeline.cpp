#include "pipeline.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace engine {

std::vector<char> Pipeline::readFile(const std::string& filePath) {
  std::ifstream file{filePath, std::ios::ate | std::ios::binary};

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filePath);
  }

  size_t fileSize = static_cast<size_t>(file.tellg());
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();
  return buffer;
}

void Pipeline::createGraphicsPipeline(
  const std::string& vertFilepath,
  const std::string& fragFilepath
) {
  auto vertCode = readFile(vertFilepath);
  auto fragCode = readFile(fragFilepath);

  // TODO: Remove debug prints
  std::cout << "Vertex shader code size: " << vertCode.size() << std::endl;
  std::cout << "Fragment shader code size: " << fragCode.size() << std::endl;
}

Pipeline::Pipeline(
  const std::string& vertFilepath,
  const std::string& fragFilepath
) {
  createGraphicsPipeline(vertFilepath, fragFilepath);
}

}
