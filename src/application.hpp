#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <string>

struct GLFWwindow;

class Application
{
public:
    Application(int window_width, int window_height, const std::string& window_title);
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

private:
    const int width_;
    const int height_;
    GLFWwindow* window_{nullptr};
};

#endif // APPLICATION_HPP