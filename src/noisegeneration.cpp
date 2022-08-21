#include "noisegeneration.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <limits>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/random.hpp>

PerlinNoiseInfo::PerlinNoiseInfo(std::uint32_t map_width, std::uint32_t map_height): 
    width{map_width}, height{map_height} 
{}

void RegionsInfo::compute_uint8_colors()
{
    for (std::size_t i = 0; i < size; ++i)
    {
        std::transform(colors[i].cbegin(), colors[i].cend(), colors_uint8[i].begin(), [](float channel_value)
        {
            return static_cast<std::uint8_t>(255.0f * channel_value);
        });
    }
}

Image<float> perlin_noise_height_map(const PerlinNoiseInfo& info)
{
    std::vector<glm::vec2> random_offsets(info.octaves);
    glm::vec2 offset{info.offset[0], info.offset[1]};
    for (int i = 0; i < info.octaves; ++i)
    {
        random_offsets[i] = offset + glm::vec2{glm::linearRand(-10000.0f, 10000.0f), glm::linearRand(-10000.0f, 10000.0f)};
    }

    Image<float> height_map{info.width, info.height};
    float min_height{std::numeric_limits<float>::max()};
    float max_height{std::numeric_limits<float>::lowest()};
    const float half_width{info.width / 2.0f};
    const float half_height{info.height / 2.0f};

    for (std::size_t i = 0; i < info.height; ++i)
    {
        for (std::size_t j = 0; j < info.width; ++j)
        {
            float frequency{1.0f};
            float amplitude{1.0f};
            float noise_height{0.0f};

            for (int octave = 0; octave < info.octaves; ++octave)
            {
                auto sample_point = (frequency / info.noise_scale) * glm::vec2{j - half_width, i - half_height};
                sample_point += random_offsets[octave];
                noise_height += amplitude * glm::perlin(sample_point);
                frequency *= info.lacunarity;
                amplitude *= info.persistance;
            }

            height_map.set(i, j, 0, noise_height);
            max_height = std::max(max_height, noise_height);
            min_height = std::min(min_height, noise_height);
        }
    }

    normalize_image(height_map, max_height, min_height);
    return height_map;
}

Image<std::uint8_t> perlin_noise_color_map(const PerlinNoiseInfo& info, const RegionsInfo& regions_info)
{
    const Image<float> height_map = perlin_noise_height_map(info);
    Image<std::uint8_t> color_map{info.width, info.height, 3};
    for (std::size_t i = 0; i < color_map.height(); ++i)
    {
        for (std::size_t j = 0; j < color_map.width(); ++j)
        {
            const float noise_height = height_map.get(i, j);
            auto region_iter = noise_height > 0.999f ? (regions_info.height_ranges.cend() - 1) : std::upper_bound(regions_info.height_ranges.cbegin(), regions_info.height_ranges.cend(), noise_height);
            assert(region_iter != regions_info.height_ranges.cend());
            std::size_t region_index = region_iter - regions_info.height_ranges.cbegin();
            const auto& color = regions_info.colors_uint8[region_index];
            color_map.set(i, j, color.cbegin(), color.cend());
        }
    }
    
    return color_map;
}