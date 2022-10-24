#include "application.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// GLAD must be imported before GLFW
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ctime>
#include <iostream>
#include <stdexcept>
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
    application->camera().process_mouse_scroll(static_cast<float>(y_offset));
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
            {"assets/shaders/heightmap/heightmap.glsl", Shader::Type::Compute},
        });
    heightmap_generator_->set_float_uniform("lacunarity", fractal_noise_generator_.noise_settings.lacunarity);
    heightmap_generator_->set_float_uniform("persistance", fractal_noise_generator_.noise_settings.persistance);
    heightmap_generator_->set_int_uniform("octaves", fractal_noise_generator_.noise_settings.octaves);
    heightmap_generator_->set_float_uniform("noise_scale", fractal_noise_generator_.noise_settings.noise_scale);
    heightmap_generator_->set_float_uniform("exponent", fractal_noise_generator_.noise_settings.exponent);

    terrain_heightmap_ = std::make_unique<Texture>(height_map_dim_.first, height_map_dim_.second);
    normalmap_generator_ =
        std::make_unique<ShaderProgram>(std::initializer_list<std::pair<std::string_view, Shader::Type>>{
            {"assets/shaders/heightmap/normalmap.glsl", Shader::Type::Compute},
        });
    terrain_normalmap_ = std::make_unique<Texture>(height_map_dim_.first, height_map_dim_.second);
    compute_terrain_maps();

    // Terrain textures attributes
    const Texture::Attributes terrain_texture_attributes{Texture::Attributes{.target = GL_TEXTURE_2D_ARRAY,
                                                                             .wrap_s = GL_REPEAT,
                                                                             .wrap_t = GL_REPEAT,
                                                                             .min_filter = GL_LINEAR_MIPMAP_LINEAR,
                                                                             .pixel_data_format = GL_RGB,
                                                                             .generate_mipmap = true,
                                                                             .layers = 3}};

    std::vector<std::string_view> albedo_names{
        "assets/textures/terrain/albedo/river_rock1_albedo.png",
        "assets/textures/terrain/albedo/slate2-tiled-albedo2.png",
        "assets/textures/terrain/albedo/rock-snow-ice1-2k_Base_Color.png",
    };
    terrain_albedos_ =
        std::make_unique<Texture>(create_arraytexture_from_file(albedo_names, terrain_texture_attributes));

    std::vector<std::string_view> normal_names{
        "assets/textures/terrain/normal/river_rock1_Normal-dx.png",
        "assets/textures/terrain/normal/slate2-tiled-normal3-UE4.png",
        "assets/textures/terrain/normal/rock-snow-ice1-2k_Normal-dx.png",
    };
    terrain_normal_maps_ =
        std::make_unique<Texture>(create_arraytexture_from_file(normal_names, terrain_texture_attributes));

    auto ambient_occlusion_attributes = terrain_texture_attributes;
    ambient_occlusion_attributes.internal_format = GL_R8;
    ambient_occlusion_attributes.pixel_data_format = GL_RED;
    std::vector<std::string_view> ao_names{
        "assets/textures/terrain/ao/river_rock1_ao.png",
        "assets/textures/terrain/ao/slate2-tiled-ao.png",
        "assets/textures/terrain/ao/rock-snow-ice1-2k_Ambient_Occlusion.png",
    };
    terrain_ao_maps_ = std::make_unique<Texture>(create_arraytexture_from_file(ao_names, ambient_occlusion_attributes));

    terrain_program_ = std::make_unique<ShaderProgram>(std::initializer_list<std::pair<std::string_view, Shader::Type>>{
        {"assets/shaders/gpu_terrain/vertex_shader.vs", Shader::Type::Vertex},
        {"assets/shaders/gpu_terrain/tess_control_shader.tcs", Shader::Type::TessControl},
        {"assets/shaders/gpu_terrain/tess_eval_shader.tes", Shader::Type::TessEval},
        {"assets/shaders/gpu_terrain/fragment_shader.fs", Shader::Type::Fragment},
    });

    terrain_program_->set_float_uniform("elevation", terrain_elevation_);
    terrain_program_->set_float_array_uniform("triplanar_scale[0]", textures_scale_.data(),
                                              static_cast<GLsizei>(textures_scale_.size()));
    terrain_program_->set_float_array_uniform("start_heights[0]", textures_start_height_.data(),
                                              static_cast<GLsizei>(textures_start_height_.size()));
    terrain_program_->set_float_array_uniform("blend_end[0]", textures_blend_end_.data(),
                                              static_cast<GLsizei>(textures_blend_end_.size()));
    terrain_program_->set_vec4_uniform("clip_plane", glm::vec4{0.0f, 0.0f, 0.0f, 0.0f});

    terrain_heightmap_->bind(0);
    terrain_normalmap_->bind(1);
    terrain_albedos_->bind(2);
    terrain_ao_maps_->bind(3);
    terrain_normal_maps_->bind(4);

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
    terrain_normal_maps_.reset();
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
        terrain_program_->set_vec3_uniform("light.diffuse", light_.diffuse);
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
    terrain_normal_maps_->bind(4);

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

    ImGui::Begin("Settings");
    if (ImGui::TreeNode("Noise"))
    {
        if (ImGui::SliderFloat("Lacunarity", &fractal_noise_generator_.noise_settings.lacunarity, 0.01f, 10.0f))
        {
            compute_terrain_maps();
        }
        if (ImGui::SliderFloat("Persistance", &fractal_noise_generator_.noise_settings.persistance, 0.01f, 1.0f))
        {
            compute_terrain_maps();
        }
        if (ImGui::SliderInt("Octaves", &fractal_noise_generator_.noise_settings.octaves, 1, 16))
        {
            compute_terrain_maps();
        }
        if (ImGui::SliderFloat("Noise Scale", &fractal_noise_generator_.noise_settings.noise_scale, 0.01f, 10.0f))
        {
            compute_terrain_maps();
        }
        if (ImGui::SliderFloat("Redistribution", &fractal_noise_generator_.noise_settings.exponent, 1.0f, 2.0f))
        {
            compute_terrain_maps();
        }
        if (ImGui::SliderInt("Seed", &fractal_noise_generator_.noise_settings.seed, -1, 100))
        {
            if (fractal_noise_generator_.noise_settings.seed == -1)
            {
                std::srand(std::time(nullptr));
            }
            else
            {
                std::srand(fractal_noise_generator_.noise_settings.seed);
            }
            fractal_noise_generator_.generate_random_offsets();
            compute_terrain_maps();
        }
        ImGui::Text("(Note: Set seed = -1 to use current time as seed)");
        if (ImGui::SliderFloat2("Offset", glm::value_ptr(fractal_noise_generator_.noise_settings.offset), -1000, 1000))
        {
            fractal_noise_generator_.generate_random_offsets();
            compute_terrain_maps();
        }

        ImTextureID imgui_texture_id = reinterpret_cast<void*>(static_cast<std::intptr_t>(terrain_heightmap_->id()));
        ImGui::Image(imgui_texture_id, ImVec2{200, 200}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f},
                     ImVec4{1.0f, 1.0f, 1.0f, 1.0f}, ImVec4{1.0f, 1.0f, 1.0f, 0.5f});
        imgui_texture_id = reinterpret_cast<void*>(static_cast<std::intptr_t>(terrain_normalmap_->id()));
        ImGui::Image(imgui_texture_id, ImVec2{200, 200}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f},
                     ImVec4{1.0f, 1.0f, 1.0f, 1.0f}, ImVec4{1.0f, 1.0f, 1.0f, 0.5f});
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Terrain and Water"))
    {
        if (ImGui::SliderFloat("Terrain Elevation", &terrain_elevation_, 0.0f, 50.0f))
        {
            terrain_program_->set_float_uniform("elevation", terrain_elevation_);
        }
        float water_height{water_->height()};

        if (ImGui::SliderFloat("Water Height", &water_height, 0.0f, 50.0f))
        {
            water_->set_height(water_height);
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Texturing"))
    {
        if (ImGui::Checkbox("Use normal mapping", &apply_normal_map_))
        {
            terrain_program_->set_bool_uniform("apply_normal_map", apply_normal_map_);
        }
        if (ImGui::Checkbox("Use triplanar texture mapping", &use_triplanar_texturing_))
        {
            terrain_program_->set_int_uniform("use_triplanar_texturing", static_cast<int>(use_triplanar_texturing_));
        }
        if (ImGui::SliderFloat("River Rock", &textures_scale_[0], 0.02f, 1.1f))
        {
            terrain_program_->set_float_array_uniform("triplanar_scale[0]", textures_scale_.data(),
                                                      static_cast<GLsizei>(textures_scale_.size()));
        }

        ImGui::Text("River Rock Blend Start is fixed at 0.0");
        if (ImGui::SliderFloat("River Rock Blend End", &textures_blend_end_[0], 0.0, 1.0f))
        {
            terrain_program_->set_float_array_uniform("blend_end[0]", textures_blend_end_.data(),
                                                      static_cast<GLsizei>(textures_blend_end_.size()));
        }

        if (ImGui::SliderFloat("Mountain Rock", &textures_scale_[1], 0.02f, 1.1f))
        {
            terrain_program_->set_float_array_uniform("triplanar_scale[0]", textures_scale_.data(),
                                                      static_cast<GLsizei>(textures_scale_.size()));
        }
        if (ImGui::SliderFloat("Mountain Rock Start", &textures_start_height_[1], textures_start_height_[0], 1.0f))
        {
            terrain_program_->set_float_array_uniform("start_heights[0]", textures_start_height_.data(),
                                                      static_cast<GLsizei>(textures_start_height_.size()));
        }
        if (ImGui::SliderFloat("Mountain Rock Blend End", &textures_blend_end_[1], 0.0f, 1.0f))
        {
            terrain_program_->set_float_array_uniform("blend_end[0]", textures_blend_end_.data(),
                                                      static_cast<GLsizei>(textures_blend_end_.size()));
        }

        if (ImGui::SliderFloat("Snow", &textures_scale_[2], 0.02f, 1.1f))
        {
            terrain_program_->set_float_array_uniform("triplanar_scale[0]", textures_scale_.data(),
                                                      static_cast<GLsizei>(textures_scale_.size()));
        }
        if (ImGui::SliderFloat("Snow Start", &textures_start_height_[2], 0.0f, 1.0f))
        {
            terrain_program_->set_float_array_uniform("start_heights[0]", textures_start_height_.data(),
                                                      static_cast<GLsizei>(textures_start_height_.size()));
        }
        ImGui::Text("Snow Blend End is fixed at 1.0");
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Halfspace Fog"))
    {
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
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Directional Light"))
    {
        light_.to_update = ImGui::SliderFloat3("Direction", glm::value_ptr(light_.direction), -20.0f, 20.0f);
        light_.to_update |= ImGui::SliderFloat3("Diffuse", glm::value_ptr(light_.diffuse), 0.0f, 1.0f);
        if (ImGui::Button("Reset Light"))
        {
            light_ = start_light_;
            light_.to_update = true;
        }
        ImGui::TreePop();
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::compute_terrain_maps()
{
    terrain_heightmap_->bind_image(0);
    heightmap_generator_->use();
    heightmap_generator_->set_float_uniform("lacunarity", fractal_noise_generator_.noise_settings.lacunarity);
    heightmap_generator_->set_float_uniform("persistance", fractal_noise_generator_.noise_settings.persistance);
    heightmap_generator_->set_int_uniform("octaves", fractal_noise_generator_.noise_settings.octaves);
    heightmap_generator_->set_float_uniform("noise_scale", fractal_noise_generator_.noise_settings.noise_scale);
    heightmap_generator_->set_float_uniform("exponent", fractal_noise_generator_.noise_settings.exponent);
    heightmap_generator_->set_vec2_array_uniform("offsets[0]", fractal_noise_generator_.random_offsets(),
                                                 fractal_noise_generator_.noise_settings.octaves);
    glDispatchCompute(height_map_dim_.first / 32, height_map_dim_.second / 32, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    normalmap_generator_->use();
    terrain_heightmap_->bind_image(0);
    terrain_normalmap_->bind_image(1);
    glDispatchCompute(height_map_dim_.first / 32, height_map_dim_.second / 32, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}