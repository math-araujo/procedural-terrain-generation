#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <cstdint>
#include <string_view>

#include "image.hpp"

class Texture
{
public:
    Texture(std::uint32_t width, std::uint32_t height);

    Texture(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&& other) noexcept;
    ~Texture();

    void copy_image(const Image<std::uint8_t>& image);
    void copy_image(std::string_view filename, bool flip_on_load = true);
    void copy_image(const unsigned char* image_data, std::int32_t width, std::int32_t height);
    void bind(std::uint32_t unit);
    std::uint32_t id() const;
private:
    std::uint32_t width_;
    std::uint32_t height_;
    std::uint32_t id_{0};
};

#endif // TEXTURE_HPP