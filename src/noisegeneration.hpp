#ifndef NOISE_GENERATION_HPP
#define NOISE_GENERATION_HPP

#include <array>
#include <cstdint>
#include <glm/glm.hpp>

#include "hermite.hpp"
#include "image.hpp"

/*
Class to store data related to the Fractal Brownian Motion (fBM)
Noise Generation
*/
class FractalNoiseGenerator
{
public:
    struct NoiseSettings
    {
        float exponent{1.0f};
        float noise_scale{3.0f};
        float lacunarity{2.0f};
        float persistance{0.5f};
        int octaves{8};
        glm::vec2 offset{0.0f};
        int seed{0};
    };

    NoiseSettings noise_settings{};

    FractalNoiseGenerator(std::uint32_t width, std::uint32_t height);
    FractalNoiseGenerator(const FractalNoiseGenerator&) = default;
    FractalNoiseGenerator(FractalNoiseGenerator&&) = default;
    FractalNoiseGenerator& operator=(const FractalNoiseGenerator&) = default;
    FractalNoiseGenerator& operator=(FractalNoiseGenerator&&) = default;
    ~FractalNoiseGenerator() = default;

    void update(bool apply_hermite_interpolation = false);
    void update_height_map(bool apply_hermite_interpolation = false);
    void generate_random_offsets();
    void update_normal_map();
    void reset_settings();
    const Image<float>& height_map() const;
    const Image<std::uint8_t>& color_map() const;
    const Image<std::uint8_t>& normal_map() const;
    const std::vector<glm::vec2>& random_offsets() const;

private:
    const std::uint32_t width_{0};
    const std::uint32_t height_{0};
    Image<float> height_map_;
    Image<std::uint8_t> normal_map_;
    std::vector<glm::vec2> random_offsets_;
    CubicHermiteCurve curve_{std::vector<glm::vec2>{{0.0f, 0.0f}, {0.4f, 0.1f}, {1.0f, 1.0f}},
                             std::vector<glm::vec2>{{1.0f, 0.1f}, {1.0f, 0.1f}, {0.7f, 2.0f}},
                             std::vector<float>{0.0f, 0.4f, 1.0f}};

    float clamp_at_edge_height(int row, int column);
    glm::vec3 cast_normal_to_rgb(glm::vec3 vector);
};

#endif // NOISE_GENERATION_HPP