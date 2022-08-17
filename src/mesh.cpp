#include "mesh.hpp"

#include <glad/glad.h>

Mesh::Mesh(std::vector<float> vertices) : number_of_vertices_{static_cast<int>(vertices.size()) / 6}
{
    glGenVertexArrays(1, &vertex_array_identifier_);
    glBindVertexArray(vertex_array_identifier_);

    // Create vertex buffer, allocate memory and copy vertices data to the device
    std::uint32_t vertex_buffer_identifier{0};
    glGenBuffers(1, &vertex_buffer_identifier);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_identifier);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(vertices.size() * sizeof(float)), vertices.data(),
                 GL_STATIC_DRAW);

    // Specify vertex format containing position and colors, respectively
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

Mesh::Mesh(Mesh&& other) noexcept :
    number_of_vertices_{other.number_of_vertices_}, vertex_array_identifier_{other.vertex_array_identifier_}
{
    other.vertex_array_identifier_ = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    std::swap(vertex_array_identifier_, other.vertex_array_identifier_);
    std::swap(number_of_vertices_, other.number_of_vertices_);
    return *this;
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vertex_array_identifier_);
    vertex_array_identifier_ = 0;
}

void Mesh::bind()
{
    glBindVertexArray(vertex_array_identifier_);
}

void Mesh::render()
{
    bind();
    glDrawArrays(GL_TRIANGLES, 0, number_of_vertices_);
}