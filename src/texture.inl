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

template <typename T>
void Texture::copy_image_array(const std::vector<T*> image_data, std::int32_t width, std::int32_t height)
{
    if (attributes_.target != GL_TEXTURE_2D_ARRAY)
    {
        throw std::invalid_argument("Expects a GL_TEXTURE_2D_ARRAY");
    }

    for (std::size_t layer = 0; layer < image_data.size(); ++layer)
    {
        glTextureSubImage3D(id_, 0, 0, 0, layer, width, height, 1, attributes_.pixel_data_format,
                            attributes_.pixel_data_type, image_data[layer]);
    }

    generate_mipmap();
}