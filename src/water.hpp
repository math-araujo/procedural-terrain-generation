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

    void bind_reflection();
    void bind_refraction();
    void unbind();

    float height() const;
    const glm::vec4& reflection_clip_plane() const;
    const glm::vec4& refraction_clip_plane() const;
    std::uint32_t reflection_color_attachment() const;
    std::uint32_t refraction_color_attachment() const;
    std::uint32_t refraction_depth_texture() const;
private:
    const std::uint32_t reflection_width_{320};
    const std::uint32_t reflection_height_{180};
    const std::uint32_t refraction_width_{1280};
    const std::uint32_t refraction_height_{720};
    const float height_{10.0f};
    const glm::vec4 reflection_clip_plane_{0.0f, 1.0f, 0.0f, -height_};
    const glm::vec4 refraction_clip_plane_{0.0f, -1.0f, 0.0f, height_};
    std::unique_ptr<Framebuffer> reflection_fbo_{};
    std::unique_ptr<Framebuffer> refraction_fbo_{};
};

#endif // WATER_HPP