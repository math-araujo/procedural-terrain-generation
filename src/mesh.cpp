#include "mesh.hpp"

#include <glad/glad.h>

Mesh::Mesh(std::vector<float> vertices) : number_of_vertices_{static_cast<int>(vertices.size()) / 5}
{
    glGenVertexArrays(1, &vertex_array_identifier_);
    glBindVertexArray(vertex_array_identifier_);

    // Create vertex buffer, allocate memory and copy vertices data to the device
    std::uint32_t vertex_buffer_identifier{0};
    glGenBuffers(1, &vertex_buffer_identifier);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_identifier);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(vertices.size() * sizeof(float)), vertices.data(),
                 GL_STATIC_DRAW);

    // Specify vertex format containing position and texture coordinates, respectively
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
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

IndexedMesh::IndexedMesh(std::vector<float> vertices_data, std::vector<std::uint32_t> indices): 
    number_of_vertices_{static_cast<int>(vertices_data.size() / 5)}, 
    number_of_indices_{static_cast<int>(indices.size())}
{
    glCreateVertexArrays(1, &vertex_array_identifier_);
    glBindVertexArray(vertex_array_identifier_);

    glGenBuffers(1, &vertex_buffer_identifier_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_identifier_);
    glBufferData(GL_ARRAY_BUFFER, vertices_data.size() * sizeof(float), vertices_data.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &element_buffer_object_id_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_id_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(std::uint32_t), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

IndexedMesh::IndexedMesh(IndexedMesh&& mesh) noexcept: 
    number_of_vertices_{mesh.number_of_vertices_}, number_of_indices_{mesh.number_of_indices_},
    vertex_array_identifier_{mesh.vertex_array_identifier_}, vertex_buffer_identifier_{mesh.vertex_buffer_identifier_},
    element_buffer_object_id_{mesh.element_buffer_object_id_}
{
    mesh.number_of_vertices_ = 0;
    mesh.number_of_indices_ = 0;
    mesh.vertex_array_identifier_ = 0;
    mesh.vertex_buffer_identifier_ = 0;
    mesh.element_buffer_object_id_ = 0;
}

IndexedMesh& IndexedMesh::operator=(IndexedMesh&& mesh) noexcept
{
    std::swap(number_of_vertices_, mesh.number_of_vertices_);
    std::swap(number_of_indices_, mesh.number_of_indices_);
    std::swap(vertex_array_identifier_, mesh.vertex_array_identifier_);
    std::swap(vertex_buffer_identifier_ , mesh.vertex_buffer_identifier_);
    std::swap(element_buffer_object_id_, mesh.element_buffer_object_id_);
    return *this;
}

IndexedMesh::~IndexedMesh()
{
    glDeleteBuffers(1, &element_buffer_object_id_);
    glDeleteBuffers(1, &vertex_buffer_identifier_);
    glDeleteVertexArrays(1, &vertex_array_identifier_);
}

void IndexedMesh::bind()
{
    glBindVertexArray(vertex_array_identifier_);
}

void IndexedMesh::render()
{
    glDrawElements(GL_TRIANGLES, number_of_indices_, GL_UNSIGNED_INT, 0);
}