#include "noisegeneration.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <limits>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>

FractalNoiseGenerator::FractalNoiseGenerator(std::uint32_t width, std::uint32_t height) :
    width_{width}, height_{height}, height_map_{width, height}, normal_map_{width, height, 4}
{
    random_offsets_.reserve(4 * noise_settings.octaves);
}

void FractalNoiseGenerator::update(bool apply_hermite_interpolation)
{
    update_height_map(apply_hermite_interpolation);
    update_normal_map();
}

void FractalNoiseGenerator::update_height_map(bool apply_hermite_interpolation)
{
    generate_random_offsets();

    float min_height{std::numeric_limits<float>::max()};
    float max_height{std::numeric_limits<float>::lowest()};
    const float half_width{width_ / 2.0f};
    const float half_height{height_ / 2.0f};

    for (std::size_t i = 0; i < height_; ++i)
    {
        for (std::size_t j = 0; j < width_; ++j)
        {
            float frequency{1.0f};
            float amplitude{1.0f};
            float noise_height{0.0f};

            for (int octave = 0; octave < noise_settings.octaves; ++octave)
            {
                auto sample_point =
                    (frequency / noise_settings.noise_scale) * glm::vec2{j - half_width, i - half_height};
                sample_point += frequency * random_offsets_[octave];
                noise_height += amplitude * glm::perlin(sample_point);
                frequency *= noise_settings.lacunarity;
                amplitude *= noise_settings.persistance;
            }

            height_map_.set(i, j, 0, noise_height);
            max_height = std::max(max_height, noise_height);
            min_height = std::min(min_height, noise_height);
        }
    }

    normalize_image(height_map_, max_height, min_height);

    if (apply_hermite_interpolation)
    {
        height_map_.transform([&curve = curve_](float noise_height) { return curve.evaluate(noise_height).y; });
    }
}

void FractalNoiseGenerator::generate_random_offsets()
{
    random_offsets_.clear();
    random_offsets_.resize(noise_settings.octaves);
    for (int i = 0; i < noise_settings.octaves; ++i)
    {
        const glm::vec2 random_sample{glm::linearRand(-10000.0f, 10000.0f), -glm::linearRand(-10000.0f, 10000.0f)};
        random_offsets_[i] = noise_settings.offset + random_sample;
    }
}

void FractalNoiseGenerator::update_normal_map()
{
    const float dz = 1.0f / 2.0f;
    const auto float_to_uint = [](float channel_value) { return static_cast<std::uint8_t>(channel_value); };
    for (int i = 0; i < static_cast<int>(height_); ++i)
    {
        for (int j = 0; j < static_cast<int>(width_); ++j)
        {
            const float top_left = clamp_at_edge_height(i - 1, j - 1);
            const float top = clamp_at_edge_height(i - 1, j);
            const float top_right = clamp_at_edge_height(i - 1, j + 1);
            const float left = clamp_at_edge_height(i, j - 1);
            const float right = clamp_at_edge_height(i, j + 1);
            const float bottom_left = clamp_at_edge_height(i + 1, j - 1);
            const float bottom = clamp_at_edge_height(i + 1, j);
            const float bottom_right = clamp_at_edge_height(i + 1, j + 1);
            const float dx = (top_right + 2 * right + bottom_right) - (top_left + 2 * left + bottom_left);
            const float dy = (bottom_left + 2 * bottom + bottom_right) - (top_left + 2 * top + top_right);
            const glm::vec3 rgb_normal = cast_normal_to_rgb(glm::normalize(glm::vec3{dx, dy, dz}));
            const glm::vec4 rgba_normal = glm::vec4{rgb_normal, 255.0};
            normal_map_.set_transform(i, j, glm::value_ptr(rgba_normal), glm::value_ptr(rgba_normal) + 4,
                                      float_to_uint);
        }
    }
}

float FractalNoiseGenerator::clamp_at_edge_height(int row, int column)
{
    row = std::clamp(row, 0, static_cast<int>(height_ - 1));
    column = std::clamp(column, 0, static_cast<int>(width_ - 1));
    return height_map_.get(static_cast<std::size_t>(row), static_cast<std::size_t>(column));
}

glm::vec3 FractalNoiseGenerator::cast_normal_to_rgb(glm::vec3 vector)
{
    return ((vector + 1.0f) / 2.0f) * 255.0f;
}

void FractalNoiseGenerator::reset_settings()
{
    static const NoiseSettings default_noise_settings{};
    noise_settings = default_noise_settings;
    update();
}

const Image<float>& FractalNoiseGenerator::height_map() const
{
    return height_map_;
}

const Image<std::uint8_t>& FractalNoiseGenerator::normal_map() const
{
    return normal_map_;
}

const std::vector<glm::vec2>& FractalNoiseGenerator::random_offsets() const
{
    return random_offsets_;
}