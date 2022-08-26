#ifndef MESH_HPP
#define MESH_HPP

#include <cstdint>
#include <vector>

class Mesh
{
public:
    Mesh() = default;
    explicit Mesh(std::vector<float> vertices);
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&& other) noexcept;
    ~Mesh();

    void bind();
    void render();

private:
    int number_of_vertices_{0};
    std::uint32_t vertex_array_identifier_{0};
};

class IndexedMesh
{
public:
    IndexedMesh(std::vector<float> vertices_data, std::vector<std::uint32_t> indices);
    
    IndexedMesh(const IndexedMesh&) = delete;
    IndexedMesh(IndexedMesh&& mesh) noexcept;
    IndexedMesh& operator=(const IndexedMesh&) = delete;
    IndexedMesh& operator=(IndexedMesh&& mesh) noexcept;
    ~IndexedMesh();

    void bind();
    void render();
    void update_mesh(std::vector<float> vertices_data, std::vector<std::uint32_t> indices);
private:
    int number_of_vertices_{0};
    int number_of_indices_{0};
    std::uint32_t vertex_array_identifier_{0};
    std::uint32_t vertex_buffer_identifier_{0};
    std::uint32_t element_buffer_object_id_{0};
};

#endif // MESH_HPP
