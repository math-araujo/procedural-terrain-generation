#ifndef WATER_HPP
#define WATER_HPP

#include <cstdint>

#include <memory>

#include <glm/glm.hpp>


class Framebuffer;

class Water
{
public:
    Water();
    Water(const Water&) = default;
    Water(Water&&) = default;
    Water& operator=(const Water&) = default;
    Water& operator=(Water&&) = default;
    ~Water() = default;

    void update(float delta_time);
    void bind_reflection();
    void bind_refraction();
    void unbind();

    float height() const;
    float dudv_offset() const;
    const glm::vec4& reflection_clip_plane() const;
    const glm::vec4& refraction_clip_plane() const;
    std::uint32_t reflection_color_attachment() const;
    std::uint32_t refraction_color_attachment() const;
    std::uint32_t refraction_depth_texture() const;
    void bind_textures();
private:
    const std::uint32_t reflection_width_{320};
    const std::uint32_t reflection_height_{180};
    const std::uint32_t refraction_width_{1280};
    const std::uint32_t refraction_height_{720};
    const float height_{10.0f};
    const float wave_speed_{.03f};
    const glm::vec4 reflection_clip_plane_{0.0f, 1.0f, 0.0f, -height_ + 0.1f};
    const glm::vec4 refraction_clip_plane_{0.0f, -1.0f, 0.0f, height_};
    std::unique_ptr<Framebuffer> reflection_fbo_{};
    std::unique_ptr<Framebuffer> refraction_fbo_{};
    float dudv_offset_{0.0f};
};

#endif // WATER_HPP