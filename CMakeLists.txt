cmake_minimum_required(VERSION 3.20)
 # Add vcpkg as package manager and install dependencies
include(cmake/fetchvcpkg.cmake)
project(procedural-terrain LANGUAGES CXX)

# Dependencies
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")

add_subdirectory(src)