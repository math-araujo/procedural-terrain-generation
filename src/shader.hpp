#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

class Shader
{
public:
    enum class Type : GLenum
    {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER
    };

    Shader(const std::string& shader_source_code, Type type);
    Shader(const Shader&) = delete;
    Shader(Shader&& shader) noexcept;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&& shader) noexcept;
    ~Shader();

    std::uint32_t identifier() const;

private:
    std::uint32_t identifier_{0};

    static const std::string& shader_typename(Type type);
};

void check_shader_compilation(std::uint32_t shader_id, std::string_view shader_type);
Shader load_shader_from_file(std::string_view filepath, Shader::Type type);

template <typename T>
constexpr std::underlying_type_t<T> to_underlying(T enumerator) noexcept
{
    return static_cast<std::underlying_type_t<T>>(enumerator);
}

#endif // SHADER_HPP