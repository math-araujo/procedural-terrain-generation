#include "application.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// GLAD must be imported before GLFW
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <exception>
#include <iostream>

Application::Application(int window_width, int window_height, std::string_view title) :
    width_{window_width}, height_{window_height}, aspect_ratio_{static_cast<float>(width_) / height_}
{
    create_context(title);
    initialize_imgui();
    load_opengl();
    mesh_ = std::make_unique<Mesh>(
        std::vector<float>
        {
            // X     Y     Z     U     V
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // Top-right
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // Top-left
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Bottom-right
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Bottom-right
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // Top-left
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // Bottom-left
        }
    );
    
    texture_ = std::make_unique<Texture>(200, 100);
    color_map_ = perlin_noise_color_map(perlin_info_);
    save_image("perlin_noise.png", color_map_);
    texture_->copy_image(color_map_);
    shader_program_ = std::make_unique<ShaderProgram>(
        std::initializer_list<std::pair<std::string_view, Shader::Type>>
        {
            {"shaders/vertex_shader.vs", Shader::Type::Vertex},
            {"shaders/fragment_shader.fs", Shader::Type::Fragment},
        }
    );
    
    shader_program_->use();
    shader_program_->set_int_uniform("texture_sampler", 0);
    mesh_->bind();
    texture_->bind(0);
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
    glfwSetWindowSizeCallback(window_, framebuffer_size_callback);
    glfwSetWindowUserPointer(window_, this);
    glfwSetCursorPosCallback(window_, mouse_movement_callback);
    glfwSetScrollCallback(window_, scroll_callback);
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_movement_callback(GLFWwindow* window, double x_pos, double y_pos)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    static bool first_mouse{true};
    static glm::vec2 last_position{0.0f, 0.0f};

    if (first_mouse)
    {
        last_position = {static_cast<float>(x_pos), static_cast<float>(y_pos)};
        first_mouse = false;
    }
    
    const float x_offset{static_cast<float>(x_pos - last_position.x)};
    const float y_offset{static_cast<float>(last_position.y - y_pos)};
    last_position = {static_cast<float>(x_pos), static_cast<float>(y_pos)};
    application->camera().process_mouse_movement(x_offset, y_offset);
}

FPSCamera& Application::camera()
{
    return camera_;
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->camera().process_mouse_scroll(y_offset);
}

void Application::initialize_imgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io; // Use io in a statement to avoid unused variable warning
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("# version 450");
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
    cleanup();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void Application::cleanup()
{
    shader_program_.reset();
    texture_.reset();
    mesh_.reset();
}

void Application::run()
{
    float delta_time = 0.0f;
    float previous_time = 0.0f;

    while (!glfwWindowShouldClose(window_))
    {
        float current_time = static_cast<float>(glfwGetTime());
        delta_time = current_time - previous_time;
        previous_time = current_time;

        process_input(delta_time);
        update();
        render();
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}

void Application::process_input(float delta_time)
{
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window_, true);
        return;
    }

    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera_.process_keyboard_input(CameraMovement::Forward, delta_time);
    }
    
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera_.process_keyboard_input(CameraMovement::Backward, delta_time);
    }
    
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera_.process_keyboard_input(CameraMovement::Right, delta_time);
    }

    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera_.process_keyboard_input(CameraMovement::Left, delta_time);
    }
}

void Application::update()
{}

void Application::render()
{
    // Clear window with specified color
    glClearColor(0.0f, 0.1f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Render scene
    projection_matrix_ = glm::perspective(glm::radians(camera_.zoom()), aspect_ratio_, 0.1f, 100.0f);
    shader_program_->set_mat4_uniform("proj_view_transform", projection_matrix_ * camera_.view());
    mesh_->render();

    // Render GUI
    render_imgui_editor();
}

void Application::render_imgui_editor()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Perlin Noise Settings");
    ImGui::SliderFloat("Lacunarity", &perlin_info_.lacunarity, 0.01f, 10.0f);
    ImGui::SliderFloat("Persistance", &perlin_info_.persistance, 0.01f, 1.0f);
    ImGui::SliderInt("Octaves", &perlin_info_.octaves, 1, 16);
    ImGui::SliderFloat("Noise Scale", &perlin_info_.noise_scale, 0.01f, 50.0f);
    ImGui::SliderFloat2("Offset", perlin_info_.offset.data(), -1000, 1000);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, 
                ImGui::GetIO().Framerate);
    if (ImGui::Button("Reset Settings"))
    {
        perlin_info_ = default_perlin_info_;
        color_map_ = perlin_noise_color_map(perlin_info_);
        texture_->copy_image(color_map_);
    }
    if (ImGui::Button("Upload"))
    {
        color_map_ = perlin_noise_color_map(perlin_info_);
        texture_->copy_image(color_map_);
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
