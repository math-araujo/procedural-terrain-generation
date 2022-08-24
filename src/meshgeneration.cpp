#include "meshgeneration.hpp"

#include <cassert>

std::unique_ptr<IndexedMesh> grid_mesh(int width, int height, const Image<float>& height_map)
{
    std::vector<float> vertices_data;
    vertices_data.reserve(width * height);
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            vertices_data.emplace_back(static_cast<float>(j) - static_cast<float>(width) / 2.0f); // x-coordinate
            //vertices_data.emplace_back(0.25); // y-coordinate // TODO: put height here
            const float map_height = height_map.get(i, j);
            assert(map_height >= 0.0f);
            assert(map_height <= 1.0f);
            vertices_data.emplace_back(16.0 * map_height); // y-coordinate // TODO: put height here
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

    return std::make_unique<IndexedMesh>(std::move(vertices_data), std::move(indices));
}