cmake_minimum_required(VERSION 3.20)
project(Mocca LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(dependencies)
include(imgui)
include(shaders)

add_subdirectory(Mocca)
add_subdirectory(Sandbox)
