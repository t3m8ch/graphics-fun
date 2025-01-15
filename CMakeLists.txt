cmake_minimum_required(VERSION 3.10)
project(GraphicsFun)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(src)
file(GLOB SOURCES src/*.cpp)

include_directories(/usr/include)

add_executable(GraphicsFun ${SOURCES})

target_link_libraries(GraphicsFun Vulkan::Vulkan glfw)
