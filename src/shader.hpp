#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <glm/fwd.hpp>

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

class Shader
{
public:
    enum class Type : GLenum
    {
        Vertex = GL_VERTEX_SHADER,
        TessControl = GL_TESS_CONTROL_SHADER,
        TessEval = GL_TESS_EVALUATION_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Compute = GL_COMPUTE_SHADER,
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

class ShaderProgram
{
public:
    ShaderProgram() = default;
    explicit ShaderProgram(std::initializer_list<std::pair<std::string_view, Shader::Type>> initializer);
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&& other) noexcept;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;
    ~ShaderProgram();

    void use();
    void set_bool_uniform(const std::string& uniform_name, bool value);
    void set_int_uniform(const std::string& uniform_name, int value);
    void set_int_array_uniform(const std::string& uniform_name, const int* value, GLsizei count);
    void set_float_uniform(const std::string& uniform_name, float value);
    void set_float_array_uniform(const std::string& uniform_name, const float* value, GLsizei count);
    void set_vec2_uniform(const std::string& uniform_name, float x, float y);
    void set_vec2_uniform(const std::string& uniform_name, const glm::vec2& vector);
    void set_vec2_array_uniform(const std::string& uniform_name, const std::vector<glm::vec2>& value, GLsizei count);
    void set_vec3_uniform(const std::string& uniform_name, float x, float y, float z);
    void set_vec3_uniform(const std::string& uniform_name, const glm::vec3& vector);
    void set_vec4_uniform(const std::string& uniform_name, const glm::vec4& vector);
    void set_mat4_uniform(const std::string& uniform_name, const glm::mat4& transform);

private:
    std::uint32_t program_id_{0};
    std::unordered_map<std::string, std::uint32_t> uniform_locations_{};

    void retrieve_uniforms();
};

// Auxiliary free functions
void check_shader_compilation(std::uint32_t shader_id, std::string_view shader_type);
Shader load_shader_from_file(std::string_view filepath, Shader::Type type);
void check_shader_program_link_status(std::uint32_t shader_program_id,
                                      std::initializer_list<std::pair<std::string_view, Shader::Type>> shader_data);
std::string process_shader_include(std::string shader_source, std::filesystem::path shader_path);

template <typename T>
constexpr std::underlying_type_t<T> to_underlying(T enumerator) noexcept
{
    return static_cast<std::underlying_type_t<T>>(enumerator);
}

#endif // SHADER_HPP