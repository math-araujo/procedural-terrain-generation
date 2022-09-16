#ifndef MESH_HPP
#define MESH_HPP

#include <cstdint>
#include <vector>

class Mesh
{
public:
    // Default vertex attributes: position (3) + texture coordinates (2)
    explicit Mesh(std::vector<float> vertices_data, std::vector<int> attributes_sizes = {3, 2});
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&& other) noexcept;
    virtual ~Mesh();

    void bind();
    virtual void render();

    int number_of_vertices() const;
    int number_of_attributes() const;
private:
    std::vector<int> attributes_sizes_{};
    int stride_{0};
    int number_of_vertices_{0};
    std::uint32_t vertex_array_identifier_{0};
};

class PatchMesh: public Mesh
{
public:
    PatchMesh(int vertices_per_patch, std::vector<float> vertices_data);

    PatchMesh(const PatchMesh&) = delete;
    PatchMesh(PatchMesh&&) = default;
    PatchMesh& operator=(const PatchMesh&) = delete;
    PatchMesh& operator=(PatchMesh&&) = default;
    ~PatchMesh() override = default;

    void render() override;
private:
    int vertices_per_patch_{0};
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
