#ifndef WATER_HPP
#define WATER_HPP

#include <cstdint>
#include <memory>
#include <string_view>

#include <glm/glm.hpp>

#include "framebuffer.hpp"
#include "mesh.hpp"
#include "renderbuffer.hpp"
#include "shader.hpp"
#include "texture.hpp"

struct DirectionalLight;
class FPSCamera;

class Water
{
public:
    Water(int plane_scale);
    Water(const Water&) = delete;
    Water(Water&&) = default;
    Water& operator=(const Water&) = delete;
    Water& operator=(Water&&) = default;
    ~Water() = default;

    void update(float delta_time);
    void render(FPSCamera& camera);

    void bind_reflection();
    void bind_refraction();
    void unbind();

    float height() const;
    void set_height(float new_height);
    float dudv_offset() const;
    const glm::vec4& reflection_clip_plane() const;
    const glm::vec4& refraction_clip_plane() const;
    std::uint32_t reflection_color_attachment() const;
    std::uint32_t refraction_color_attachment() const;
    std::uint32_t refraction_depth_texture() const;

    void update_light(const DirectionalLight& light);

private:
    const std::uint32_t reflection_width_{1024};
    const std::uint32_t reflection_height_{768};
    const std::uint32_t refraction_width_{1024};
    const std::uint32_t refraction_height_{768};
    const float wave_speed_{.03f};
    float height_{16.5f};
    glm::vec4 reflection_clip_plane_{0.0f, 1.0f, 0.0f, -height_ + 0.1f};
    glm::vec4 refraction_clip_plane_{0.0f, -1.0f, 0.0f, height_ + 0.2f};
    glm::mat4 model_{1.0f};
    float dudv_offset_{0.0f};
    float plane_scale_{1.0f};

    IndexedMesh mesh_{std::vector<float>{
                          // X     Y     Z     U     V
                          0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // Top-right
                          -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, // Top-left
                          0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // Bottom-right
                          -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // Bottom-left
                      },
                      std::vector<std::uint32_t>{0, 1, 2, 2, 1, 3}};

    ShaderProgram shader_program_{std::initializer_list<std::pair<std::string_view, Shader::Type>>{
        {"assets/shaders/water/vertex_shader.vs", Shader::Type::Vertex},
        {"assets/shaders/water/fragment_shader.fs", Shader::Type::Fragment},
    }};
    Texture dudv_map_{create_texture_from_file("assets/textures/water/dudv.png",
                                               Texture::Attributes{.wrap_s = GL_REPEAT, .wrap_t = GL_REPEAT})};
    Texture normal_map_{create_texture_from_file("assets/textures/water/normal.png",
                                                 Texture::Attributes{.wrap_s = GL_REPEAT, .wrap_t = GL_REPEAT})};

    // Reflection uses renderbuffer for depth buffer and texture for color buffer
    Framebuffer reflection_fbo_{
        reflection_width_, reflection_height_,
        Renderbuffer{reflection_width_, reflection_height_, GL_DEPTH_COMPONENT32},
        Texture{reflection_width_, reflection_height_, Texture::Attributes{.wrap_s = GL_REPEAT, .wrap_t = GL_REPEAT}}};

    // Refraction uses texture for both depth and colors buffers
    Framebuffer refraction_fbo_{
        refraction_width_, refraction_height_,
        Texture{refraction_width_, refraction_height_,
                Texture::Attributes{.internal_format = GL_DEPTH_COMPONENT32,
                                    .pixel_data_format = GL_DEPTH_COMPONENT,
                                    .pixel_data_type = GL_FLOAT}},
        Texture{refraction_width_, refraction_height_, Texture::Attributes{.wrap_s = GL_REPEAT, .wrap_t = GL_REPEAT}}};

    void compute_model_matrix();
};

#endif // WATER_HPP