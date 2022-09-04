#include "texture.hpp"

#include <algorithm>
#include <cassert>
#include <glad/glad.h>
#include <stb_image.h>

Texture::Texture(std::uint32_t width, std::uint32_t height, Attributes attributes):
    width_{width}, height_{height}, attributes_{attributes}
{
    initialize();
}

void Texture::initialize()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &id_);
    glTextureParameteri(id_, GL_TEXTURE_WRAP_S, attributes_.wrap_s);
    glTextureParameteri(id_, GL_TEXTURE_WRAP_T, attributes_.wrap_t);
    glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, attributes_.min_filter);
    glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, attributes_.mag_filter);
    glTextureStorage2D(id_, 1, attributes_.internal_format, width_, height_);
}

Texture::Texture(std::uint32_t width, std::uint32_t height): 
    width_{width}, height_{height}
{
    initialize();
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

std::uint32_t Texture::id() const
{
    return id_;
}

void Texture::copy_image(std::string_view filename, bool flip_on_load)
{
    int width{0};
    int height{0};
    int number_of_channels{0};
    stbi_set_flip_vertically_on_load(flip_on_load);
    unsigned char* data = stbi_load(filename.data(), &width, &height, &number_of_channels, 0);
    assert(data != nullptr);
    if (number_of_channels == 3)
    {
        attributes_.pixel_data_format = GL_RGB;
    }
    copy_image(data, width, height);
    stbi_image_free(data);
}