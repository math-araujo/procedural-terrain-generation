#ifndef NOISE_GENERATION_HPP
#define NOISE_GENERATION_HPP

#include <array>
#include <cstdint>
#include <glm/glm.hpp>

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
        float noise_scale{30.0f};
        float lacunarity{2.0f};
        float persistance{0.5f};
        int octaves{8};
        glm::vec2 offset{0.0f};
    };

    struct RegionsSettings
    {
        inline static constexpr std::size_t size{8};
        std::array<float, size> height_ranges{0.3f, 0.4f, 0.45f, 0.55f, 0.6f, 0.7f, 0.85f, 1.0f};
        std::array<std::array<float, 3>, size> colors
        {{
            {0.01568627450980392, 0.23529411764705882, 0.7529411764705882},
            {0.01568627450980392, 0.23529411764705882, 0.7529411764705882},
            {0.8352941176470589, 0.7176470588235294, 0.5450980392156862},
            {0.33725490196078434, 0.596078431372549, 0.09019607843137255},
            {0.00784313725490196, 0.23137254901960785, 0.027450980392156862},
            {0.23529411764705882, 0.12941176470588237, 0.011764705882352941},
            {0.15294117647058825, 0.06666666666666667, 0.00392156862745098},
            {1.0, 1.0, 1.0}
        }};
        std::array<std::array<std::uint8_t, 4>, size> colors_uint8
        {{
            // Deep Water  Shallow Water   Sand             Grass
            //{1, 73, 244}, {4, 60, 192}, {213, 183, 139}, {86, 152, 23},
            {4, 60, 192, 255}, {4, 60, 192, 255}, {213, 183, 139, 0}, {86, 152, 23, 255},
            // Dark Grass Rock       Mountain        Snow
            {2, 59, 7, 255}, {60, 33, 3, 255}, {39, 17, 1, 0}, {255, 255, 255, 255}
        }};
        const std::array<std::string, size> names
        {
            "Deep Water", "Shallow Water", "Sand", "Grass", "Dark Grass", "Rock", "Mountain", "Snow"
        };
        
        void compute_uint8_colors();
    };

    NoiseSettings noise_settings{};
    RegionsSettings regions_settings{};
    
    FractalNoiseGenerator(std::uint32_t width, std::uint32_t height);
    FractalNoiseGenerator(const FractalNoiseGenerator&) = default;
    FractalNoiseGenerator(FractalNoiseGenerator&&) = default;
    FractalNoiseGenerator& operator=(const FractalNoiseGenerator&) = default;
    FractalNoiseGenerator& operator=(FractalNoiseGenerator&&) = default;
    ~FractalNoiseGenerator() = default;

    void update_height_map();
    void update_color_map();
    void reset_settings();
    const Image<float>& height_map() const;
    const Image<std::uint8_t>& color_map() const;
private:
    const std::uint32_t width_{0};
    const std::uint32_t height_{0};
    Image<float> height_map_;
    Image<std::uint8_t> color_map_;
    std::vector<glm::vec2> random_offsets_;
};

#endif // NOISE_GENERATION_HPP