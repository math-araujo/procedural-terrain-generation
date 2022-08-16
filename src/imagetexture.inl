#include "imagetexture.hpp"

#include <cassert>

template<typename RandomAccessIterator>
void ImageTexture::set(std::size_t i, std::size_t j, RandomAccessIterator begin, RandomAccessIterator end)
{
    assert(static_cast<std::size_t>(end - begin) == depth_);
    const std::size_t index = ((i * width_) + j) * depth_;
    auto iterator = image_data_.begin() + index;
    std::copy(begin, end, iterator);
}