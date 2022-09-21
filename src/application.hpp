#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <array>
#include <memory>
#include <string_view>

#include <glm/fwd.hpp>

#include "camera.hpp"
#include "image.hpp"
#include "light.hpp"
#include "noisegeneration.hpp"

struct GLFWwindow;

class IndexedMesh;
class Mesh;
class ShaderProgram;
class Skybox;
class Texture;
class Water;

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
    void process_input(float delta_time);
    void update(float delta_time);
    void render();

    // Functions to interact with GLFW callback functions
    FPSCamera& camera();
    bool is_wireframe_mode() const;
    void switch_wireframe_mode();
    void set_mouse_click(bool mouse_click);
    bool mouse_clicking() const;
    void switch_free_mouse_movement();
    bool is_mouse_movement_free() const;

private:
    const int width_;
    const int height_;
    const float aspect_ratio_;

    std::array<int, 4> current_viewport_{};
    GLFWwindow* window_{nullptr};
    bool wireframe_mode_{false};
    bool mouse_click_{false};
    bool free_mouse_move_{false};

    FPSCamera camera_{glm::vec3{0.0, 30.0f, 3.0f}};
    glm::mat4 projection_matrix_{1.0f};
    const std::pair<std::uint32_t, std::uint32_t> height_map_dim_{256, 256};
    FractalNoiseGenerator fractal_noise_generator_{height_map_dim_.first, height_map_dim_.second};
    std::unique_ptr<Mesh> mesh_{};
    // std::unique_ptr<IndexedMesh> mesh_{};
    std::unique_ptr<Texture> texture_{};
    std::unique_ptr<Texture> normal_map_{};

    std::vector<std::unique_ptr<Texture>> albedos_;
    std::unique_ptr<ShaderProgram> terrain_program_{};
    float elevation_{30.0f};
    float texture_scale_{1.0f / 10.0f};
    glm::mat4 terrain_scale_{1.0f};

    const DirectionalLight start_light_{glm::vec3{-1.0f, -1.0f, -1.0f}, glm::vec3{0.2f, 0.2f, 0.2f},
                                        glm::vec3{0.85f, 0.85f, 0.85f}, glm::vec3{0.85f, 0.85f, 0.85f}, true};
    DirectionalLight light_{start_light_};
    bool use_triplanar_texturing_{false};

    std::unique_ptr<IndexedMesh> water_mesh_{};
    std::unique_ptr<ShaderProgram> water_program_{};
    std::unique_ptr<Texture> water_dudv_map_{};
    std::unique_ptr<Texture> water_normal_map_{};
    std::unique_ptr<Water> water_{};
    std::unique_ptr<Skybox> skybox_{};
    float fog_height_{20.0f};
    float fog_density_{0.001f};
    bool apply_fog_{true};

    /*
    Create a window and OpenGL context. If creation
    was unsuccesfull, throws a runtime exception.
    */
    void create_context(std::string_view title);

    /*
    Initializes ImGui
    */
    void initialize_imgui();

    /*
    Load OpenGL functions. If loading was unsuccessfull,
    throws a runtime exception.
    */
    void load_opengl();

    /*
    Render procedural terrain on GPU
    */
    void render_terrain();

    /*
    Reset viewport to the Application's width and height values
    */
    void reset_viewport();

    /*
    Render editor with properties for noise generation and
    regions colors.
    */
    void render_imgui_editor();

    /*
    Update height map and mesh.
    */
    void update_noise_and_mesh();

    /*
    Manually cleanup OpenGL-related objects. Since Application
    destructor terminates the OpenGL context, it's necessary
    to manually cleanup all OpenGL-related objects prior to
    the termination of the context.
    */
    void cleanup();
};

void error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_movement_callback(GLFWwindow* window, double x_pos, double y_pos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);

#endif // APPLICATION_HPP