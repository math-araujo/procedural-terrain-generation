#include "application.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// GLAD must be imported before GLFW
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <exception>
#include <iostream>
#include <string> 

#include "framebuffer.hpp"
#include "mesh.hpp"
#include "meshgeneration.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "water.hpp"

Application::Application(int window_width, int window_height, std::string_view title) :
    width_{window_width}, height_{window_height}, aspect_ratio_{static_cast<float>(width_) / height_}
{
    create_context(title);
    initialize_imgui();
    load_opengl();
    /*mesh_ = std::make_unique<Mesh>(
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
    );*/
    /*mesh_ = std::make_unique<IndexedMesh>(
        std::vector<float>
        {
            // X     Y     Z     U     V
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // Top-right
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // Top-left
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Bottom-right
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // Bottom-left
        },
        std::vector<std::uint32_t>
        {
            0, 1, 2,
            2, 1, 3
        }
    );*/
    fractal_noise_generator_.update();
    save_image("heightmap.png", from_float_to_uint8(fractal_noise_generator_.height_map()));
    save_image("normalmap.png", fractal_noise_generator_.normal_map());
    save_image("biomemap.png", fractal_noise_generator_.color_map());
    //mesh_ = create_indexed_grid_mesh(200, 200, fractal_noise_generator_.height_map(), curve_);
    mesh_ = create_grid_patch(height_map_dim_.first, height_map_dim_.second, 32);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_CLIP_DISTANCE0);
    texture_ = std::make_unique<Texture>(height_map_dim_.first, height_map_dim_.second);
    texture_->copy_image(fractal_noise_generator_.color_map());
    normal_map_ = std::make_unique<Texture>(height_map_dim_.first, height_map_dim_.second);
    normal_map_->copy_image(fractal_noise_generator_.normal_map());
    albedos_.reserve(5);
    for (int i = 0; i < 5; ++i)
    {
        albedos_.emplace_back(std::make_unique<Texture>(
            height_map_dim_.first, height_map_dim_.second,
            Texture::Attributes
            {
                .wrap_s = GL_REPEAT, .wrap_t = GL_REPEAT, .generate_mipmap = true,
            }
        ));
    }
    std::array<std::string, 5> names
    {
        "textures/water.png", "textures/sand.png", "textures/grass.png",
        "textures/rock.png", "textures/snow.png",
    };
    for (std::size_t i = 0; i < albedos_.size(); ++i)
    {
        albedos_[i]->copy_image(names[i]);
    }
    /*terrain_program_ = std::make_unique<ShaderProgram>(
        std::initializer_list<std::pair<std::string_view, Shader::Type>>
        {
            {"shaders/cpu_terrain/vertex_shader.vs", Shader::Type::Vertex},
            {"shaders/cpu_terrain/fragment_shader.fs", Shader::Type::Fragment},
        }
    );*/
    terrain_program_ = std::make_unique<ShaderProgram>(
        std::initializer_list<std::pair<std::string_view, Shader::Type>>
        {
            {"shaders/gpu_terrain/vertex_shader.vs", Shader::Type::Vertex},
            {"shaders/gpu_terrain/tess_control_shader.tcs", Shader::Type::TessControl},
            {"shaders/gpu_terrain/tess_eval_shader.tes", Shader::Type::TessEval},
            {"shaders/gpu_terrain/fragment_shader.fs", Shader::Type::Fragment},
        }
    );

    terrain_program_->use();
    terrain_program_->set_int_uniform("texture_sampler", 0);
    terrain_program_->set_int_uniform("normal_map_sampler", 1);
    terrain_program_->set_float_uniform("elevation", elevation_);
    terrain_program_->set_vec4_uniform("clip_plane", glm::vec4{0.0f, -1.0f, 0.0f, 15.0f});

    texture_->bind(0);
    normal_map_->bind(1);
    const std::array<int, 5> uniforms{2, 3, 4, 5, 6};
    terrain_program_->set_int_array_uniform("albedos[0]", uniforms.data(), uniforms.size());
    for (std::size_t i = 0; i < albedos_.size(); ++i)
    { 
        albedos_[i]->bind(i + 2);
    }

    terrain_program_->set_int_uniform("use_triplanar_texturing", static_cast<int>(use_triplanar_texturing_));

    water_mesh_ = std::make_unique<IndexedMesh>(
        std::vector<float>
        {
            // X     Y     Z     U     V
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // Top-right
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // Top-left
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Bottom-right
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // Bottom-left
        },
        std::vector<std::uint32_t>
        {
            0, 1, 2,
            2, 1, 3
        }
    );
    water_program_ = std::make_unique<ShaderProgram>(
        std::initializer_list<std::pair<std::string_view, Shader::Type>>
        {
            {"shaders/water/vertex_shader.vs", Shader::Type::Vertex},
            {"shaders/water/fragment_shader.fs", Shader::Type::Fragment},
        }
    );

    water_fbo_ = std::make_unique<Water>();
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
    glfwSetKeyCallback(window_, key_callback);
    glfwSetCursorPosCallback(window_, mouse_movement_callback);
    glfwSetMouseButtonCallback(window_, mouse_button_callback);
    glfwSetScrollCallback(window_, scroll_callback);
}

void error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        if (application->is_wireframe_mode())
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        application->switch_wireframe_mode();
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        if (application->is_mouse_movement_free())
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        application->switch_free_mouse_movement();
    }
}

bool Application::is_wireframe_mode() const
{
    return wireframe_mode_;
}

void Application::switch_wireframe_mode()
{
    wireframe_mode_ = !wireframe_mode_;
}

bool Application::is_mouse_movement_free() const
{
    return free_mouse_move_;
}

void Application::switch_free_mouse_movement()
{
    free_mouse_move_ = !free_mouse_move_;
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
    if (application->is_mouse_movement_free() || application->mouse_clicking())
    {
        application->camera().process_mouse_movement(x_offset, y_offset);
    }
}

bool Application::mouse_clicking() const
{
    return mouse_click_;
}

FPSCamera& Application::camera()
{
    return camera_;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        switch (action)
        {
        case GLFW_PRESS:
            application->set_mouse_click(true);
            break;
        case GLFW_RELEASE:
            application->set_mouse_click(false);
            break;
        default:
            break;
        }
    }
}

void Application::set_mouse_click(bool mouse_click)
{
    mouse_click_ = mouse_click;
}

void scroll_callback(GLFWwindow* window, double /*x_offset*/, double y_offset)
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
    water_fbo_.reset();
    water_program_.reset();
    water_mesh_.reset();

    terrain_program_.reset();
    for (auto& albedo: albedos_)
    {
        albedo.reset();
    }
    normal_map_.reset();
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

    if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera_.process_keyboard_input(CameraMovement::Up, delta_time);
    }

    if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera_.process_keyboard_input(CameraMovement::Down, delta_time);
    }
}

void Application::update()
{
    if (light_.to_update)
    {
        terrain_program_->set_vec3_uniform("light.direction", light_.direction);    
        terrain_program_->set_vec3_uniform("light.ambient", light_.ambient);
        terrain_program_->set_vec3_uniform("light.diffuse", light_.diffuse);
        light_.to_update = false;
    }
}

void Application::render()
{
    glGetIntegerv(GL_VIEWPORT, current_viewport_.data());

    // Render scene to the reflection framebuffer
    // The clip plane must be above water surface
    // Camera must be positioned below water surface
    terrain_program_->set_vec4_uniform("clip_plane", water_fbo_->reflection_clip_plane());
    const float underwater_distance{2.0f * (camera_.position().y - water_fbo_->height())};
    camera_.move_position(glm::vec3{0.0f, -underwater_distance, 0.0f});
    camera_.invert_pitch();
    water_fbo_->bind_reflection();
    render_terrain();

    // Render scene to the refraction
    // The clip plane must be below water surface
    // Camera position and orientation is restored to the previous values
    camera_.move_position(glm::vec3{0.0f, underwater_distance, 0.0f});
    camera_.invert_pitch();
    terrain_program_->set_vec4_uniform("clip_plane", water_fbo_->refraction_clip_plane());
    water_fbo_->bind_refraction();
    render_terrain();

    // Reset viewport and bind default framebuffer
    water_fbo_->unbind();
    reset_viewport();

    // Clear window with specified color
    //glClearColor(0.0f, 0.1f, 0.4f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render scene
    terrain_program_->set_vec4_uniform("clip_plane", glm::vec4{0.0f, 0.0f, 0.0f, 0.0f});
    render_terrain();

    // Render water
    water_program_->use();
    glm::mat4 model = glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f, water_fbo_->height(), 0.0f});
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3{1.0f, 0.0f, 0.0f});
    model = glm::scale(model, glm::vec3{height_map_dim_.first, height_map_dim_.second, 1.0f});
    water_program_->set_mat4_uniform("mvp", projection_matrix_ * camera_.view() * model);
    water_mesh_->render();

    // Render GUI
    render_imgui_editor();
}

void Application::render_terrain()
{
    projection_matrix_ = glm::perspective(glm::radians(camera_.zoom()), aspect_ratio_, 0.1f, 1000.0f);
    terrain_program_->use();
    terrain_program_->set_mat4_uniform("model", terrain_scale_);
    terrain_program_->set_mat4_uniform("model_view", camera_.view() * terrain_scale_);
    terrain_program_->set_mat4_uniform("mvp", projection_matrix_ * camera_.view() * terrain_scale_);
    mesh_->render();
}


void Application::reset_viewport()
{
    glViewport(current_viewport_[0], current_viewport_[1], current_viewport_[2], current_viewport_[3]);
}

void Application::render_imgui_editor()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Fractal Noise Settings");
    ImGui::SliderFloat("Lacunarity", &fractal_noise_generator_.noise_settings.lacunarity, 0.01f, 10.0f);
    ImGui::SliderFloat("Persistance", &fractal_noise_generator_.noise_settings.persistance, 0.01f, 1.0f);
    ImGui::SliderInt("Octaves", &fractal_noise_generator_.noise_settings.octaves, 1, 16);
    ImGui::SliderFloat("Noise Scale", &fractal_noise_generator_.noise_settings.noise_scale, 0.01f, 50.0f);
    ImGui::SliderFloat2("Offset", glm::value_ptr(fractal_noise_generator_.noise_settings.offset), -1000, 1000);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, 
                ImGui::GetIO().Framerate);
    ImTextureID imgui_texture_id = reinterpret_cast<void*>(texture_->id());
    ImGui::Image(imgui_texture_id, ImVec2{200, 200}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f}, 
                ImVec4{1.0f, 1.0f, 1.0f, 1.0f}, ImVec4{1.0f, 1.0f, 1.0f, 0.5f});
    imgui_texture_id = reinterpret_cast<void*>(normal_map_->id());
    ImGui::Image(imgui_texture_id, ImVec2{200, 200}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f}, 
                ImVec4{1.0f, 1.0f, 1.0f, 1.0f}, ImVec4{1.0f, 1.0f, 1.0f, 0.5f});

    if (ImGui::Button("Reset Settings"))
    {
        fractal_noise_generator_.reset_settings();
        update_noise_and_mesh();
    }
    if (ImGui::Button("Upload"))
    {
        fractal_noise_generator_.update();
        update_noise_and_mesh();
    }
    ImGui::End();

    /*ImGui::Begin("Regions Settings");
    ImGui::SliderFloat(fractal_noise_generator_.regions_settings.names[0].c_str(), 
        fractal_noise_generator_.regions_settings.height_ranges.data(), 0.05f, 1.0f);
    ImGui::ColorPicker3(fractal_noise_generator_.regions_settings.names[0].c_str(), 
                        fractal_noise_generator_.regions_settings.colors[0].data());
    for (std::size_t i = 1; i < fractal_noise_generator_.regions_settings.size; ++i)
    {
        ImGui::PushID(i);
        ImGui::SliderFloat(fractal_noise_generator_.regions_settings.names[i].c_str(), 
            &fractal_noise_generator_.regions_settings.height_ranges[i], 
            fractal_noise_generator_.regions_settings.height_ranges[i - 1] + 0.01f, 1.0f);
        ImGui::ColorPicker3("Color", fractal_noise_generator_.regions_settings.colors[i].data());
        ImGui::PopID();
    }
    ImGui::End();*/

    ImGui::Begin("Light Settings");
    light_.to_update = ImGui::SliderFloat3("Direction", glm::value_ptr(light_.direction), -20.0f, 20.0f);
    light_.to_update |= ImGui::SliderFloat3("Ambient", glm::value_ptr(light_.ambient), 0.0f, 1.0f);
    light_.to_update |= ImGui::SliderFloat3("Diffuse", glm::value_ptr(light_.diffuse), 0.0f, 1.0f);
    if (ImGui::Button("Reset Light"))
    {
        light_ = start_light_;
        light_.to_update = true;
    }
    ImGui::End();

    ImGui::Begin("Texturing");
    if (ImGui::Checkbox("Use triplanar texture mapping", &use_triplanar_texturing_))
    {
        terrain_program_->set_int_uniform("use_triplanar_texturing", static_cast<int>(use_triplanar_texturing_));
    }
    ImGui::End();

    ImGui::Begin("Terrain");
    if (ImGui::SliderFloat("Elevation", &elevation_, 1.0f, 300.0f))
    {
        terrain_program_->set_float_uniform("elevation", elevation_);
    }
    ImGui::End();

    ImGui::Begin("Water");
    ImGui::Text("Reflection:");
    imgui_texture_id = reinterpret_cast<void*>(water_fbo_->reflection_color_attachment());
    ImGui::Image(imgui_texture_id, ImVec2{200, 200}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f}, 
                ImVec4{1.0f, 1.0f, 1.0f, 1.0f}, ImVec4{1.0f, 1.0f, 1.0f, 0.5f});
    ImGui::Text("Refraction:");
    imgui_texture_id = reinterpret_cast<void*>(water_fbo_->refraction_color_attachment());
    ImGui::Image(imgui_texture_id, ImVec2{200, 200}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f}, 
                ImVec4{1.0f, 1.0f, 1.0f, 1.0f}, ImVec4{1.0f, 1.0f, 1.0f, 0.5f});
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::update_noise_and_mesh()
{
    texture_->copy_image(fractal_noise_generator_.color_map());
    normal_map_->copy_image(fractal_noise_generator_.normal_map());
    //auto grid_mesh_data = grid_mesh(height_map_dim_.first, height_map_dim.second, fractal_noise_generator_.height_map(), curve_);
    //mesh_->update_mesh(std::move(grid_mesh_data.first), std::move(grid_mesh_data.second));
}