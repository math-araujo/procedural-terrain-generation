#include "application.hpp"

#include <GLFW/glfw3.h>

Application::Application(int window_width, int window_height, std::string_view title) :
    width_{window_width}, height_{window_height}
{}