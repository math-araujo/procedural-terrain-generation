#ifndef IMAGE_TEXTURE_HPP
#define IMAGE_TEXTURE_HPP

#include <cstddef>
#include <vector>

/* 
Class representing 3D image texture data stored using an interleaved
format (e.g. RGBRGB or RGBARGBA).
*/
class ImageTexture
{
public:
    ImageTexture(std::size_t width, std::size_t height, std::size_t depth = 1);

    ImageTexture(const ImageTexture&) = default;
    ImageTexture(ImageTexture&&) noexcept = default;
    ImageTexture& operator=(const ImageTexture&) = default;
    ImageTexture& operator=(ImageTexture&&) noexcept = default;
    ~ImageTexture() = default;

    unsigned char get(std::size_t i, std::size_t j, std::size_t k = 0) const;

    template<typename RandomAccessIterator>
    void set(std::size_t i, std::size_t j, RandomAccessIterator begin, RandomAccessIterator end);
    void set(std::size_t i, std::size_t j, std::size_t k, unsigned char value);
    
    const unsigned char* data() const;
    std::size_t width() const;
    std::size_t height() const;
    std::size_t depth() const;
private:
    std::size_t width_{0};
    std::size_t height_{0};
    std::size_t depth_{1};
    std::vector<unsigned char> image_data_;
};

#include "imagetexture.inl"

#endif // IMAGE_TEXTURE_HPP