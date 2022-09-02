#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <cstdint>
#include <string_view>

#include <glad/glad.h>

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

    template<typename T>
    void copy_image(const Image<T>& image);

    template<typename T>
    void copy_image(const T* image_data, std::int32_t width, std::int32_t height);

    void copy_image(std::string_view filename, bool flip_on_load = true);
    void bind(std::uint32_t unit);
    std::uint32_t id() const;
private:
    std::uint32_t width_;
    std::uint32_t height_;
    std::uint32_t id_{0};
};

#include "texture.inl"

#endif // TEXTURE_HPP