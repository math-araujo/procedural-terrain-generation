#include "image.hpp"

#include <algorithm>
#include <cassert>
#include <type_traits>

template<typename T>
Image<T>::Image(std::size_t width, std::size_t height, std::size_t depth):
    width_{width}, height_{height}, depth_{depth}, image_data_(width * height * depth)
{
    static_assert(std::is_arithmetic_v<T>, "ImageTexture underlying type must be numeric");
}

template<typename T>
T Image<T>::get(std::size_t i, std::size_t j, std::size_t k) const
{
    std::size_t index = ((i * width_) + j) * depth_ + k;
    return image_data_[index];
}

template<typename T>
template<typename RandomAccessIterator>
void Image<T>::set(std::size_t i, std::size_t j, RandomAccessIterator begin, RandomAccessIterator end)
{
    assert(static_cast<std::size_t>(end - begin) == depth_);
    const std::size_t index = ((i * width_) + j) * depth_;
    auto iterator = image_data_.begin() + index;
    std::copy(begin, end, iterator);
}

template<typename T>
void Image<T>::set(std::size_t i, std::size_t j, std::size_t k, T value)
{
    std::size_t index = ((i * width_) + j) * depth_ + k;
    image_data_[index] = value;
}

template<typename T>
template<typename RandomAccessIterator, typename Function>
void Image<T>::set_transform(std::size_t i, std::size_t j, RandomAccessIterator begin, RandomAccessIterator end, Function&& function)
{
    assert(static_cast<std::size_t>(end - begin) == depth_);
    const std::size_t index = ((i * width_) + j) * depth_;
    auto iterator = image_data_.begin() + index;
    std::transform(begin, end, iterator, function);
}

template<typename T>
template<typename Function>
void Image<T>::transform(Function&& function)
{
    std::transform(image_data_.begin(), image_data_.end(), image_data_.begin(), function);
}

template<typename T>
const T* Image<T>::data() const
{
    return image_data_.data();
}

template<typename T>
auto Image<T>::begin()
{
    return image_data_.begin();
}

template<typename T>
auto Image<T>::end()
{
    return image_data_.end();
}

template<typename T>
auto Image<T>::cbegin() const
{
    return image_data_.cbegin();
}

template<typename T>
auto Image<T>::cend() const
{
    return image_data_.cend();
}

template<typename T>
std::size_t Image<T>::width() const
{
    return width_;
}

template<typename T>
std::size_t Image<T>::height() const
{
    return height_;
}

template<typename T>
std::size_t Image<T>::depth() const
{
    return depth_;
}

template<typename T>
std::size_t Image<T>::pixels() const
{
    return width_ * height_ * depth_;
}

template<typename T>
T Image<T>::max() const
{
    return *std::max_element(image_data_.begin(), image_data_.end());
}

template<typename T>
T Image<T>::min() const
{
    return *std::min_element(image_data_.begin(), image_data_.end());
}