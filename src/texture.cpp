#include "texture.hpp"

#include <algorithm>
#include <cassert>
#include <glad/glad.h>
#include <stb_image.h>

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

void Texture::copy_image(const Image<std::uint8_t>& image)
{
    copy_image(static_cast<const unsigned char*>(image.data()), image.width(), image.height());
}

void Texture::copy_image(const unsigned char* image_data, std::int32_t width, std::int32_t height)
{
    glTextureSubImage2D(id_, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image_data);
}

void Texture::copy_image(std::string_view filename, bool flip_on_load)
{
    int width{0};
    int height{0};
    int number_of_channels{0};
    stbi_set_flip_vertically_on_load(flip_on_load);
    unsigned char* data = stbi_load(filename.data(), &width, &height, &number_of_channels, 0);
    assert(data != nullptr);
    copy_image(data, width, height);
    stbi_image_free(data);
}