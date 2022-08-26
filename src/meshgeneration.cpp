#include "meshgeneration.hpp"

#include <cassert>

#include "hermite.hpp"
#include "mesh.hpp"

std::pair<std::vector<float>, std::vector<std::uint32_t>> grid_mesh(int width, int height, const Image<float>& height_map, const CubicHermiteCurve& curve)
{
    std::vector<float> vertices_data;
    vertices_data.reserve(width * height);
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            vertices_data.emplace_back(static_cast<float>(j) - static_cast<float>(width) / 2.0f); // x-coordinate
            const float map_height = height_map.get(i, j);
            assert(map_height >= 0.0f);
            assert(map_height <= 1.0f);
            const glm::vec2 hermite_height = curve.evaluate(map_height);
            assert(hermite_height.y >= 0.0f);
            vertices_data.emplace_back(15.0 * hermite_height.y); // y-coordinate
            vertices_data.emplace_back(static_cast<float>(i) - static_cast<float>(height) / 2.0f); // z-coordinate
            vertices_data.emplace_back(static_cast<float>(j) / width); // U-texture coordinate
            vertices_data.emplace_back(static_cast<float>(i) / height); // V-texture coordinate
        }
    }

    std::vector<std::uint32_t> indices;
    indices.reserve((width - 1) * (height - 1) * 6); // 2 triangles per square, 3 indices per triangle
    for (int i = 0; i < height - 1; ++i)
    {
        for (int j = 0; j < width - 1; ++j)
        {
            // Upper triangle of the quad
            indices.emplace_back(j + (i * width));
            indices.emplace_back(j + ((i + 1) * width) + 1);
            indices.emplace_back(j + (i * width) + 1);

            // Lower triangle of the quad
            indices.emplace_back(j + (i * width));
            indices.emplace_back(j + ((i + 1) * width));
            indices.emplace_back(j + ((i + 1) * width) + 1);
        }
    }

    return {std::move(vertices_data), std::move(indices)};
}

std::unique_ptr<IndexedMesh> create_indexed_grid_mesh(int width, int height, const Image<float>& height_map, const CubicHermiteCurve& curve)
{
    auto grid_mesh_data = grid_mesh(width, height, height_map, curve);
    return std::make_unique<IndexedMesh>(std::move(grid_mesh_data.first), std::move(grid_mesh_data.second));
}