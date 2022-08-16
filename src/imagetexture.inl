#include "imagetexture.hpp"

#include <cassert>
#include <type_traits>

template<typename T>
ImageTexture<T>::ImageTexture(std::size_t width, std::size_t height, std::size_t depth):
    width_{width}, height_{height}, depth_{depth}, image_data_(width * height * depth)
{
    static_assert(std::is_arithmetic_v<T>, "ImageTexture underlying type must be numeric");
}

template<typename T>
T ImageTexture<T>::get(std::size_t i, std::size_t j, std::size_t k) const
{
    std::size_t index = ((i * width_) + j) * depth_ + k;
    return image_data_[index];
}

template<typename T>
template<typename RandomAccessIterator>
void ImageTexture<T>::set(std::size_t i, std::size_t j, RandomAccessIterator begin, RandomAccessIterator end)
{
    assert(static_cast<std::size_t>(end - begin) == depth_);
    const std::size_t index = ((i * width_) + j) * depth_;
    auto iterator = image_data_.begin() + index;
    std::copy(begin, end, iterator);
}

template<typename T>
void ImageTexture<T>::set(std::size_t i, std::size_t j, std::size_t k, T value)
{
    std::size_t index = ((i * width_) + j) * depth_ + k;
    image_data_[index] = value;
}

template<typename T>
const T* ImageTexture<T>::data() const
{
    return image_data_.data();
}

template<typename T>
std::size_t ImageTexture<T>::width() const
{
    return width_;
}

template<typename T>
std::size_t ImageTexture<T>::height() const
{
    return height_;
}

template<typename T>
std::size_t ImageTexture<T>::depth() const
{
    return depth_;
}