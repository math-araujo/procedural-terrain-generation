#include "application.hpp"

#include <GLFW/glfw3.h>

Application::Application(int window_width, int window_height, const std::string& window_title)
    : width_{window_width}, height_{window_height}
{
}