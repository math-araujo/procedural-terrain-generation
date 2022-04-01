#ifndef MESH_HPP
#define MESH_HPP

#include <cstdint>
#include <vector>

class Mesh
{
public:
    explicit Mesh(std::vector<float> vertices);
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&& other) noexcept;
    ~Mesh();

    void bind();
    void render();

private:
    int number_of_vertices_;
    std::uint32_t vertex_array_identifier_{0};
};

#endif // MESH_HPP
