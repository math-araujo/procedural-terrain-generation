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

    std::uint32_t reflection_color_attachment() const;
    std::uint32_t refraction_color_attachment() const;
private:
    const std::uint32_t reflection_width_{320};
    const std::uint32_t reflection_height_{180};
    const std::uint32_t refraction_width_{1280};
    const std::uint32_t refraction_height_{720};
    std::unique_ptr<Framebuffer> reflection_fbo_{};
    std::unique_ptr<Framebuffer> refraction_fbo_{};
};

#endif // WATER_HPP