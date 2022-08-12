#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <string_view>

#include "mesh.hpp"

struct GLFWwindow;

class Application
{
public:
    Application(int window_width, int window_height, std::string_view title);
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;
    ~Application();

    void run();
    void process_input();
    void update();
    void render();

private:
    const int width_;
    const int height_;
    GLFWwindow* window_{nullptr};

    /*
    Create a window and OpenGL context. If creation
    was unsuccesfull, throws a runtime exception.
    */
    void create_context(std::string_view title);

    /*
    Load OpenGL functions. If loading was unsuccessfull,
    throws a runtime exception.
    */
    void load_opengl();
};

void error_callback(int error, const char* description);

#endif // APPLICATION_HPP