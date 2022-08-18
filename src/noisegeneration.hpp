#ifndef NOISE_GENERATION_HPP
#define NOISE_GENERATION_HPP

#include <array>
#include <cstdint>
#include <glm/glm.hpp>

#include "image.hpp"

struct PerlinNoiseInfo
{
    PerlinNoiseInfo(std::uint32_t map_width, std::uint32_t map_height);

    float noise_scale{27.3f};
    float lacunarity{2.0f};
    float persistance{0.5f};
    int octaves{8};
    std::array<float, 2> offset;
    std::uint32_t width{200};
    std::uint32_t height{100};
};

Image<float> perlin_noise_height_map(const PerlinNoiseInfo& info);
Image<std::uint8_t> perlin_noise_color_map(const PerlinNoiseInfo& info);

#endif // NOISE_GENERATION_HPP