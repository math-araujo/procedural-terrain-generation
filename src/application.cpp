#include "application.hpp"

// GLAD must be imported before GLFW
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <exception>
#include <iostream>

Application::Application(int window_width, int window_height, std::string_view title) :
    width_{window_width}, height_{window_height}
{
    create_context(title);
    load_opengl();
}

void Application::create_context(std::string_view title)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        glfwTerminate();
        throw std::runtime_error("Failure to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(width_, height_, title.data(), nullptr, nullptr);
    if (!window_)
    {
        glfwTerminate();
        throw std::runtime_error("Failure to create OpenGL context or GLFW window");
    }

    glfwMakeContextCurrent(window_);
}

void error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

void Application::load_opengl()
{
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        glfwDestroyWindow(window_);
        glfwTerminate();
        throw std::runtime_error("Failure to initialize GLAD");
    }
}

Application::~Application()
{
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void Application::run()
{
    while (!glfwWindowShouldClose(window_))
    {
        process_input();
        update();
        render();
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}

void Application::process_input()
{}

void Application::update()
{}

void Application::render()
{
    // Clear window with specified color
    glClearColor(0.0f, 0.1f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render scene
}