#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <string_view>
#include <memory>

#include "mesh.hpp"
#include "shader.hpp"

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

    std::unique_ptr<Mesh> mesh_{};
    std::unique_ptr<ShaderProgram> shader_program_{};

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

    /*
    Manually cleanup OpenGL-related objects. Since Application
    destructor terminates the OpenGL context, it's necessary
    to manually cleanup all OpenGL-related objects prior to 
    the termination of the context.
    */
    void cleanup();
};

void error_callback(int error, const char* description);

#endif // APPLICATION_HPP