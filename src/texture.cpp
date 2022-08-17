#include "texture.hpp"

#include <algorithm>
#include <glad/glad.h>

Texture::Texture(std::uint32_t width, std::uint32_t height): 
    width_{width}, height_{height}
{
    glCreateTextures(GL_TEXTURE_2D, 1, &id_);
    glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureStorage2D(id_, 1, GL_RGB8, width_, height_);   
}

Texture::Texture(const Image<std::uint8_t>& image): 
    Texture{static_cast<std::uint8_t>(image.width()), static_cast<std::uint8_t>(image.height())}
{
    copy_image(image);
}

void Texture::copy_image(const Image<std::uint8_t>& image)
{
    glTextureSubImage2D(id_, 0, 0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, image.data());
}

Texture::Texture(Texture&& other) noexcept: 
    width_{other.width_}, height_{other.height_}, id_{other.id_}
{
    other.id_ = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    std::swap(width_, other.width_);
    std::swap(height_, other.height_);
    std::swap(id_, other.id_);
    return *this;
}

Texture::~Texture()
{
    glDeleteTextures(1, &id_);
}

void Texture::bind(std::uint32_t unit)
{
    glBindTextureUnit(unit, id_);
}