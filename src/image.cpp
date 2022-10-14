#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

void save_image(std::string_view filename, const Image<std::uint8_t>& image)
{
    stbi_write_png(filename.data(), static_cast<int>(image.width()), static_cast<int>(image.height()),
                   static_cast<int>(image.depth()), image.data(), static_cast<int>(image.width() * image.depth()));
}

void normalize_image(Image<float>& image)
{
    normalize_image(image, image.max(), image.min());
}

void normalize_image(Image<float>& image, float max, float min)
{
    const float range{max - min};
    for (float& value : image)
    {
        value = (value - min) / range;
    }
}

Image<std::uint8_t> from_float_to_uint8(const Image<float>& image)
{
    Image<std::uint8_t> new_image(image.width(), image.height(), image.depth());
    for (std::size_t i = 0; i < image.height(); ++i)
    {
        for (std::size_t j = 0; j < image.width(); ++j)
        {
            for (std::size_t k = 0; k < image.depth(); ++k)
            {
                new_image.set(i, j, k, static_cast<std::uint8_t>(255.0f * image.get(i, j, k)));
            }
        }
    }
    return new_image;
}