#include "shader.hpp"

#include <array>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

Shader::Shader(const std::string& shader_source_code, Type type) : identifier_{glCreateShader(to_underlying(type))}
{
    const char* source_code_ptr = shader_source_code.c_str();
    glShaderSource(identifier_, 1, &source_code_ptr, nullptr);
    glCompileShader(identifier_);
    check_shader_compilation(identifier_, shader_typename(type));
}

void check_shader_compilation(std::uint32_t shader_id, std::string_view shader_type)
{
    int compilation_successful = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compilation_successful);

    if (!compilation_successful)
    {
        std::array<char, 1024> error_log{};
        glGetShaderInfoLog(shader_id, static_cast<GLsizei>(error_log.size()), nullptr, error_log.data());
        std::stringstream stream;
        stream << shader_type.data() << " Shader compilation error:\n" << error_log.data() << "\n";
        throw std::runtime_error(stream.str());
    }
}

Shader::Shader(Shader&& shader) noexcept : identifier_{shader.identifier_}
{
    shader.identifier_ = 0;
}

Shader& Shader::operator=(Shader&& shader) noexcept
{
    std::swap(identifier_, shader.identifier_);
    return *this;
}

Shader::~Shader()
{
    glDeleteShader(identifier_);
}

std::uint32_t Shader::identifier() const
{
    return identifier_;
}

const std::string& Shader::shader_typename(Shader::Type type)
{
    static const std::vector<std::string> shader_types = {"Vertex", "Fragment"};
    return shader_types[to_underlying(type)];
}

Shader load_shader_from_file(std::string_view filepath, Shader::Type type)
{
    std::ifstream shader_file{filepath.data()};
    if (!shader_file.is_open())
    {
        std::stringstream error_log_stream;
        error_log_stream << "File " << filepath << " could not be opened";
        throw std::runtime_error(error_log_stream.str());
    }

    std::stringstream source_code_stream;
    source_code_stream << shader_file.rdbuf();
    return Shader{source_code_stream.str(), type};
}