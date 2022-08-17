#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <cstdint>

#include "image.hpp"

class Texture
{
public:
    Texture(std::uint32_t width, std::uint32_t height);
    explicit Texture(const Image<std::uint8_t>& image);

    Texture(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&& other) noexcept;
    ~Texture();

    void copy_image(const Image<std::uint8_t>& image);
    void bind(std::uint32_t unit);
private:
    std::uint32_t width_;
    std::uint32_t height_;
    std::uint32_t id_{0};
};

#endif // TEXTURE_HPP