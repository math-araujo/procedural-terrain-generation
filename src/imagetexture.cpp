#include "imagetexture.hpp"

ImageTexture::ImageTexture(std::size_t width, std::size_t height, std::size_t depth):
    width_{width}, height_{height}, depth_{depth}, image_data_(width * height * depth)
{}

unsigned char ImageTexture::get(std::size_t i, std::size_t j, std::size_t k) const
{
    std::size_t index = ((i * width_) + j) * depth_ + k;
    return image_data_[index];
}

void ImageTexture::set(std::size_t i, std::size_t j, std::size_t k, unsigned char value)
{
    std::size_t index = ((i * width_) + j) * depth_ + k;
    image_data_[index] = value;
}

const unsigned char* ImageTexture::data() const
{
    return image_data_.data();
}

std::size_t ImageTexture::width() const
{
    return width_;
}

std::size_t ImageTexture::height() const
{
    return height_;
}

std::size_t ImageTexture::depth() const
{
    return depth_;
}