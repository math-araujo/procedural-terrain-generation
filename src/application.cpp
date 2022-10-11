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
#include "skybox.hpp"
#include "texture.hpp"
#include "water.hpp"

Application::Application(int window_width, int window_height, std::string_view title) :
    width_{window_width}, height_{window_height}, aspect_ratio_{static_cast<float>(width_) / height_}
{
    create_context(title);
    load_opengl();
    initialize_imgui();
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
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";

    camera_.set_aspect_ratio(aspect_ratio_);
    initialize_terrain();
    water_ = std::make_unique<Water>(grid_mesh_dim_.first);
    skybox_ = std::make_unique<Skybox>();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_CLIP_DISTANCE0);
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

void Application::load_opengl()
{
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        glfwDestroyWindow(window_);
        glfwTerminate();
        throw std::runtime_error("Failure to initialize GLAD");
    }
}

void Application::initialize_imgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io; // Use io in a statement to avoid unused variable warning
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("# version 450");
}

void Application::initialize_terrain()
{
    terrain_mesh_ = create_grid_patch(grid_mesh_dim_.first, grid_mesh_dim_.second, 64);

    heightmap_generator_ =
        std::make_unique<ShaderProgram>(std::initializer_list<std::pair<std::string_view, Shader::Type>>{
            {"shaders/heightmap/heightmap.cs", Shader::Type::Compute},
        });
    heightmap_generator_->set_float_uniform("lacunarity", fractal_noise_generator_.noise_settings.lacunarity);
    heightmap_generator_->set_float_uniform("persistance", fractal_noise_generator_.noise_settings.persistance);
    heightmap_generator_->set_float_uniform("octaves", fractal_noise_generator_.noise_settings.octaves);
    heightmap_generator_->set_float_uniform("noise_scale", fractal_noise_generator_.noise_settings.noise_scale);
    heightmap_generator_->set_float_uniform("exponent", fractal_noise_generator_.noise_settings.exponent);

    terrain_heightmap_ = std::make_unique<Texture>(height_map_dim_.first, height_map_dim_.second);
    terrain_heightmap_->bind_image(0);
    heightmap_generator_->use();
    glDispatchCompute(height_map_dim_.first / 32, height_map_dim_.second / 32, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    normalmap_generator_ =
        std::make_unique<ShaderProgram>(std::initializer_list<std::pair<std::string_view, Shader::Type>>{
            {"shaders/heightmap/normalmap.cs", Shader::Type::Compute},
        });
    terrain_normalmap_ = std::make_unique<Texture>(height_map_dim_.first, height_map_dim_.second);
    normalmap_generator_->use();
    terrain_heightmap_->bind_image(0);
    terrain_normalmap_->bind_image(1);
    glDispatchCompute(height_map_dim_.first / 32, height_map_dim_.second / 32, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // Terrain textures attributes
    const Texture::Attributes terrain_texture_attributes{Texture::Attributes{.target = GL_TEXTURE_2D_ARRAY,
                                                                             .wrap_s = GL_REPEAT,
                                                                             .wrap_t = GL_REPEAT,
                                                                             .min_filter = GL_LINEAR_MIPMAP_LINEAR,
                                                                             .pixel_data_format = GL_RGB,
                                                                             .generate_mipmap = true,
                                                                             .layers = 3}};

    terrain_albedos_ =
        std::make_unique<Texture>(grid_mesh_dim_.first, grid_mesh_dim_.second, terrain_texture_attributes);
    std::vector<std::string_view> albedo_names{
        "textures/terrain/water.png",
        "textures/terrain/rock.png",
        "textures/terrain/snow.png",
    };
    terrain_albedos_->load_array_texture(albedo_names);

    terain_normal_maps_ =
        std::make_unique<Texture>(grid_mesh_dim_.first, grid_mesh_dim_.second, terrain_texture_attributes);
    std::vector<std::string_view> normal_names{
        "textures/terrain/water_normal.png",
        "textures/terrain/rock_normal.png",
        "textures/terrain/snow_normal.png",
    };
    terain_normal_maps_->load_array_texture(normal_names);

    auto ambient_occlusion_attributes = terrain_texture_attributes;
    ambient_occlusion_attributes.internal_format = GL_R8;
    ambient_occlusion_attributes.pixel_data_format = GL_RED;
    terrain_ao_maps_ =
        std::make_unique<Texture>(grid_mesh_dim_.first, grid_mesh_dim_.second, ambient_occlusion_attributes);

    std::vector<std::string_view> ao_names{
        "textures/terrain/water_ao.png",
        "textures/terrain/rock_ao.png",
        "textures/terrain/snow_ao.png",
    };
    terrain_ao_maps_->load_array_texture(ao_names);

    terrain_program_ = std::make_unique<ShaderProgram>(std::initializer_list<std::pair<std::string_view, Shader::Type>>{
        {"shaders/gpu_terrain/vertex_shader.vs", Shader::Type::Vertex},
        {"shaders/gpu_terrain/tess_control_shader.tcs", Shader::Type::TessControl},
        {"shaders/gpu_terrain/tess_eval_shader.tes", Shader::Type::TessEval},
        {"shaders/gpu_terrain/fragment_shader.fs", Shader::Type::Fragment},
    });

    terrain_program_->set_float_uniform("elevation", terrain_elevation_);
    terrain_program_->set_float_array_uniform("triplanar_scale[0]", textures_scale_.data(), textures_scale_.size());
    terrain_program_->set_float_array_uniform("start_heights[0]", textures_start_height_.data(),
                                              textures_start_height_.size());
    terrain_program_->set_float_array_uniform("blend_end[0]", textures_blend_end_.data(), textures_blend_end_.size());
    terrain_program_->set_vec4_uniform("clip_plane", glm::vec4{0.0f, 0.0f, 0.0f, 0.0f});

    terrain_heightmap_->bind(0);
    terrain_normalmap_->bind(1);
    terrain_albedos_->bind(2);
    terrain_ao_maps_->bind(3);
    terain_normal_maps_->bind(4);

    terrain_program_->set_bool_uniform("use_triplanar_texturing", use_triplanar_texturing_);
    terrain_program_->set_bool_uniform("apply_normal_map", apply_normal_map_);
    terrain_program_->set_float_uniform("fog.height", fog_height_);
    terrain_program_->set_float_uniform("fog.density", fog_density_);
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
    skybox_.reset();
    water_.reset();
    terrain_program_.reset();
    terrain_ao_maps_.reset();
    terain_normal_maps_.reset();
    terrain_albedos_.reset();
    terrain_mesh_.reset();
    terrain_normalmap_.reset();
    terrain_heightmap_.reset();
    normalmap_generator_.reset();
    heightmap_generator_.reset();
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
        update(delta_time);
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

void Application::update(float delta_time)
{
    if (light_.to_update)
    {
        water_->update_light(light_);

        terrain_program_->use();
        terrain_program_->set_vec3_uniform("light.direction", light_.direction);
        terrain_program_->set_vec3_uniform("light.ambient", light_.ambient);
        terrain_program_->set_vec3_uniform("light.diffuse", light_.diffuse);
        terrain_program_->set_vec3_uniform("light.specular", light_.specular);
        light_.to_update = false;
    }

    water_->update(delta_time);
}

void Application::render()
{
    glGetIntegerv(GL_VIEWPORT, current_viewport_.data());

    // Render scene to the reflection framebuffer
    // The clip plane must be above water surface
    // Camera must be positioned below water surface
    terrain_program_->use();
    terrain_program_->set_vec4_uniform("clip_plane", water_->reflection_clip_plane());
    const float underwater_distance{2.0f * (camera_.position().y - water_->height())};
    camera_.move_position(glm::vec3{0.0f, -underwater_distance, 0.0f});
    camera_.invert_pitch();
    water_->bind_reflection();
    render_terrain();

    // Render scene to the refraction
    // The clip plane must be below water surface
    // Camera position and orientation is restored to the previous values
    camera_.move_position(glm::vec3{0.0f, underwater_distance, 0.0f});
    camera_.invert_pitch();
    terrain_program_->set_vec4_uniform("clip_plane", water_->refraction_clip_plane());
    water_->bind_refraction();
    render_terrain();

    // Reset viewport and bind default framebuffer
    water_->unbind();
    reset_viewport();

    // Clear window with specified color
    // glClearColor(0.0f, 0.1f, 0.4f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render scene
    terrain_program_->set_bool_uniform("apply_fog", apply_fog_);
    terrain_program_->set_vec4_uniform("clip_plane", glm::vec4{0.0f, 0.0f, 0.0f, 0.0f});
    render_terrain();

    // Render water
    water_->render(camera_);

    // Render GUI
    render_imgui_editor();
}

void Application::render_terrain()
{
    terrain_program_->use();
    terrain_heightmap_->bind(0);
    terrain_normalmap_->bind(1);
    terrain_albedos_->bind(2);
    terrain_ao_maps_->bind(3);
    terain_normal_maps_->bind(4);

    terrain_program_->set_mat4_uniform("model", terrain_scale_);
    terrain_program_->set_vec3_uniform("camera_position", camera_.position());
    terrain_program_->set_mat4_uniform("model_view", camera_.view() * terrain_scale_);
    terrain_program_->set_mat4_uniform("mvp", camera_.view_projection() * terrain_scale_);
    terrain_mesh_->render();
    skybox_->render(camera_.projection(), camera_.view());
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
    if (ImGui::SliderFloat("Lacunarity", &fractal_noise_generator_.noise_settings.lacunarity, 0.01f, 10.0f))
    {
        update_noise_and_mesh();
    }
    if (ImGui::SliderFloat("Persistance", &fractal_noise_generator_.noise_settings.persistance, 0.01f, 1.0f))
    {
        update_noise_and_mesh();
    }
    if (ImGui::SliderInt("Octaves", &fractal_noise_generator_.noise_settings.octaves, 1, 16))
    {
        update_noise_and_mesh();
    }
    if (ImGui::SliderFloat("Noise Scale", &fractal_noise_generator_.noise_settings.noise_scale, 0.01f, 50.0f))
    {
        update_noise_and_mesh();
    }
    if (ImGui::SliderFloat("Redistribution", &fractal_noise_generator_.noise_settings.exponent, 1.0f, 2.0f))
    {
        update_noise_and_mesh();
    }
    ImGui::SliderFloat2("Offset", glm::value_ptr(fractal_noise_generator_.noise_settings.offset), -1000, 1000);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImTextureID imgui_texture_id = reinterpret_cast<void*>(terrain_heightmap_->id());
    ImGui::Image(imgui_texture_id, ImVec2{200, 200}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f},
                 ImVec4{1.0f, 1.0f, 1.0f, 1.0f}, ImVec4{1.0f, 1.0f, 1.0f, 0.5f});
    imgui_texture_id = reinterpret_cast<void*>(terrain_normalmap_->id());
    ImGui::Image(imgui_texture_id, ImVec2{200, 200}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f},
                 ImVec4{1.0f, 1.0f, 1.0f, 1.0f}, ImVec4{1.0f, 1.0f, 1.0f, 0.5f});
    ImGui::End();

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
    if (ImGui::Checkbox("Use normal mapping", &apply_normal_map_))
    {
        terrain_program_->set_bool_uniform("apply_normal_map", apply_normal_map_);
    }
    if (ImGui::Checkbox("Use triplanar texture mapping", &use_triplanar_texturing_))
    {
        terrain_program_->set_int_uniform("use_triplanar_texturing", static_cast<int>(use_triplanar_texturing_));
    }

    /*
    terrain_program_->set_float_array_uniform("triplanar_scale[0]", textures_scale_.data(), textures_scale_.size());
    terrain_program_->set_float_array_uniform("start_heights[0]", textures_start_height_.data(),
                                              textures_start_height_.size());
    terrain_program_->set_float_array_uniform("blend_end[0]", textures_blend_end_.data(), textures_blend_end_.size());
    */
    if (ImGui::SliderFloat("Water", &textures_scale_[0], 0.02f, 1.1f))
    {
        terrain_program_->set_float_array_uniform("triplanar_scale[0]", textures_scale_.data(), textures_scale_.size());
    }
    if (ImGui::SliderFloat("Water Start", &textures_start_height_[0], 0.0f, 0.0f))
    {
        terrain_program_->set_float_array_uniform("start_heights[0]", textures_start_height_.data(),
                                                  textures_start_height_.size());
    }
    if (ImGui::SliderFloat("Water Blend End", &textures_blend_end_[0], 0.0, 1.0f))
    {
        terrain_program_->set_float_array_uniform("blend_end[0]", textures_blend_end_.data(),
                                                  textures_blend_end_.size());
    }
    /*if (ImGui::SliderFloat("Sand", &textures_scale_[1], 0.02f, 1.1f))
    {
        terrain_program_->set_float_array_uniform("triplanar_scale[0]", textures_scale_.data(), textures_scale_.size());
    }
    if (ImGui::SliderFloat("Grass", &textures_scale_[2], 0.02f, 1.1f))
    {
        terrain_program_->set_float_array_uniform("triplanar_scale[0]", textures_scale_.data(), textures_scale_.size());
    }*/
    if (ImGui::SliderFloat("Rock", &textures_scale_[1], 0.02f, 1.1f))
    {
        terrain_program_->set_float_array_uniform("triplanar_scale[0]", textures_scale_.data(), textures_scale_.size());
    }
    if (ImGui::SliderFloat("Rock Start", &textures_start_height_[1], 0.0f, 1.0f))
    {
        terrain_program_->set_float_array_uniform("start_heights[0]", textures_start_height_.data(),
                                                  textures_start_height_.size());
    }
    if (ImGui::SliderFloat("Rock Blend End", &textures_blend_end_[1], 0.0f, 1.0f))
    {
        terrain_program_->set_float_array_uniform("blend_end[0]", textures_blend_end_.data(),
                                                  textures_blend_end_.size());
    }

    if (ImGui::SliderFloat("Snow", &textures_scale_[2], 0.02f, 1.1f))
    {
        terrain_program_->set_float_array_uniform("triplanar_scale[0]", textures_scale_.data(), textures_scale_.size());
    }
    if (ImGui::SliderFloat("Snow Start", &textures_start_height_[2], 0.0f, 1.0f))
    {
        terrain_program_->set_float_array_uniform("start_heights[0]", textures_start_height_.data(),
                                                  textures_start_height_.size());
    }
    if (ImGui::SliderFloat("Snow Blend End", &textures_blend_end_[2], 1.1f, 1.1f))
    {
        terrain_program_->set_float_array_uniform("blend_end[0]", textures_blend_end_.data(),
                                                  textures_blend_end_.size());
    }
    ImGui::End();

    ImGui::Begin("Terrain");
    if (ImGui::SliderFloat("Elevation", &terrain_elevation_, -20.0f, 50.0f))
    {
        terrain_program_->set_float_uniform("elevation", terrain_elevation_);
    }
    ImGui::End();

    ImGui::Begin("Fog");
    ImGui::Checkbox("Apply Halfspace Fog", &apply_fog_);
    if (apply_fog_)
    {
        if (ImGui::SliderFloat("Fog Height", &fog_height_, 0.0f, 40.0f))
        {
            terrain_program_->set_float_uniform("fog.height", fog_height_);
        }
        if (ImGui::SliderFloat("Fog Density", &fog_density_, 0.001f, 0.1f))
        {
            terrain_program_->set_float_uniform("fog.density", fog_density_);
        }
    }
    ImGui::End();

    ImGui::Begin("Water");
    ImGui::Text("Reflection:");
    imgui_texture_id = reinterpret_cast<void*>(water_->reflection_color_attachment());
    ImGui::Image(imgui_texture_id, ImVec2{200, 200}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f},
                 ImVec4{1.0f, 1.0f, 1.0f, 1.0f}, ImVec4{1.0f, 1.0f, 1.0f, 0.5f});
    ImGui::Text("Refraction:");
    imgui_texture_id = reinterpret_cast<void*>(water_->refraction_color_attachment());
    ImGui::Image(imgui_texture_id, ImVec2{200, 200}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f},
                 ImVec4{1.0f, 1.0f, 1.0f, 1.0f}, ImVec4{1.0f, 1.0f, 1.0f, 0.5f});
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::update_noise_and_mesh()
{
    terrain_heightmap_->bind_image(0);
    heightmap_generator_->use();
    heightmap_generator_->set_float_uniform("lacunarity", fractal_noise_generator_.noise_settings.lacunarity);
    heightmap_generator_->set_float_uniform("persistance", fractal_noise_generator_.noise_settings.persistance);
    heightmap_generator_->set_float_uniform("octaves", fractal_noise_generator_.noise_settings.octaves);
    heightmap_generator_->set_float_uniform("noise_scale", fractal_noise_generator_.noise_settings.noise_scale);
    heightmap_generator_->set_float_uniform("exponent", fractal_noise_generator_.noise_settings.exponent);
    glDispatchCompute(height_map_dim_.first / 32, height_map_dim_.second / 32, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    normalmap_generator_->use();
    terrain_heightmap_->bind_image(0);
    terrain_normalmap_->bind_image(1);
    glDispatchCompute(height_map_dim_.first / 32, height_map_dim_.second / 32, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}