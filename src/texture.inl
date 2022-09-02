#include "texture.hpp"

template<typename T>
void Texture::copy_image(const Image<T>& image)
{
    copy_image(static_cast<const T*>(image.data()), image.width(), image.height());
}

template<typename T>
void Texture::copy_image(const T* image_data, std::int32_t width, std::int32_t height)
{
    glTextureSubImage2D(id_, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
}