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
        stream << shader_type << " Shader compilation error:\n" << error_log.data() << "\n";
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
    static const std::unordered_map<Shader::Type, std::string> shader_types = {
        {Shader::Type::Vertex, "Vertex"}, 
        {Shader::Type::Fragment, "Fragment"}
    };
    return shader_types.at(type);
}

ShaderProgram::ShaderProgram(std::initializer_list<std::pair<std::string_view, Shader::Type>> initializer):
    program_id_{glCreateProgram()}
{
    std::vector<Shader> shaders;
    shaders.reserve(initializer.size());
    for (const auto& [filepath, shader_type]: initializer)
    {
        shaders.emplace_back(load_shader_from_file(filepath, shader_type));
        glAttachShader(program_id_, shaders.back().identifier());
    }

    glLinkProgram(program_id_);
    check_shader_program_link_status(program_id_, initializer);

    for (const auto& shader: shaders)
    {
        glDetachShader(program_id_, shader.identifier());
    }

    retrieve_uniforms();
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

void check_shader_program_link_status(std::uint32_t shader_program_id, std::initializer_list<std::pair<std::string_view, Shader::Type>> shader_data)
{
    int linking_success{0};
    glGetProgramiv(shader_program_id, GL_LINK_STATUS, &linking_success);
    if (!linking_success)
    {
        std::array<char, 1024> error_log{};
        glGetProgramInfoLog(shader_program_id, static_cast<GLsizei>(error_log.size()), nullptr, error_log.data());
        std::stringstream stream;
        stream << "Shader program linking error:\n" << error_log.data() << "\nShader Program Files: ";
        for (const auto& [filepath, shader_type]: shader_data)
        {
            stream << filepath << " ";
        }
        stream << "\n";
        throw std::runtime_error(stream.str());
    }
}

void ShaderProgram::retrieve_uniforms()
{
    int number_of_uniforms{0};
    glGetProgramInterfaceiv(program_id_, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &number_of_uniforms);
    std::array<GLenum, 2> properties{GL_NAME_LENGTH, GL_LOCATION};
    std::array<GLint, 2> results{};
    std::vector<char> uniform_name(256);
    for (int uniform = 0; uniform < number_of_uniforms; ++uniform)
    {
        glGetProgramResourceiv(program_id_, GL_PROGRAM_INPUT, uniform, properties.size(), properties.data(),
                                results.size(), nullptr, results.data());
        
        // Get resources (uniform name and uniform location)
        uniform_name.resize(results[0]);
        glGetProgramResourceName(program_id_, GL_PROGRAM_INPUT, uniform, uniform_name.size(), nullptr, 
                                uniform_name.data());
        std::uint32_t uniform_location = results.back();

        // The name returned contains a null-terminator, so it's necessary to read uniform_name.size() - 1 characters
        uniform_locations.emplace(std::string{uniform_name.data(), uniform_name.size() - 1}, uniform_location);
    }
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept: program_id_{other.program_id_}
{
    other.program_id_ = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
{
    std::swap(program_id_, other.program_id_);
    return *this;
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(program_id_);
}

void ShaderProgram::use()
{
    glUseProgram(program_id_);
}