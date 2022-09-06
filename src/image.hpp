#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <cstdint>
#include <string_view>
#include <vector>

/* 
Class representing 3D image data stored using an interleaved
format (e.g. RGBRGB or RGBARGBA).
*/
template<typename T>
class Image
{
public:
    Image(std::size_t width, std::size_t height, std::size_t depth = 1);

    Image(const Image&) = default;
    Image(Image&&) noexcept = default;
    Image& operator=(const Image&) = default;
    Image& operator=(Image&&) noexcept = default;
    ~Image() = default;

    T get(std::size_t i, std::size_t j, std::size_t k = 0) const;

    template<typename RandomAccessIterator>
    void set(std::size_t i, std::size_t j, RandomAccessIterator begin, RandomAccessIterator end);
    void set(std::size_t i, std::size_t j, std::size_t k, T value);
    template<typename RandomAccessIterator, typename Function>
    void set_transform(std::size_t i, std::size_t j, RandomAccessIterator begin, RandomAccessIterator end, Function&& function);
    
    template<typename Function>
    void transform(Function && function);
    
    const T* data() const;
    auto begin();
    auto end();
    auto cbegin() const;
    auto cend() const;

    std::size_t width() const;
    std::size_t height() const;
    std::size_t depth() const;
    std::size_t pixels() const;
    T max() const;
    T min() const;
private:
    std::size_t width_{0};
    std::size_t height_{0};
    std::size_t depth_{1};
    std::vector<T> image_data_;
};

void save_image(std::string_view filename, const Image<std::uint8_t>& image);

/* 
Normalize grayscale floating-point image. 
If there's more than one image channel, assumes that
all channels are equal (e.g. for a RGB image, R = G = B).
*/
void normalize_image(Image<float>& image);
void normalize_image(Image<float>& image, float max, float min);

Image<std::uint8_t> from_float_to_uint8(const Image<float>& image);

#include "image.inl"

#endif // IMAGE_HPP