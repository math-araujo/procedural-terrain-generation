#include "texture.hpp"

template <typename T>
void Texture::copy_image(const Image<T>& image)
{
    copy_image(static_cast<const T*>(image.data()), image.width(), image.height());
}

template <typename T>
void Texture::copy_image(const T* image_data, std::int32_t width, std::int32_t height)
{
    glTextureSubImage2D(id_, 0, 0, 0, width, height, attributes_.pixel_data_format, attributes_.pixel_data_type,
                        image_data);
    generate_mipmap();
}