#include "meshgeneration.hpp"

#include <cassert>

#include "hermite.hpp"
#include "mesh.hpp"

std::pair<std::vector<float>, std::vector<std::uint32_t>> grid_mesh(int width, int height,
                                                                    const Image<float>& height_map,
                                                                    const CubicHermiteCurve& curve)
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
            vertices_data.emplace_back(15.0f * hermite_height.y);                                  // y-coordinate
            vertices_data.emplace_back(static_cast<float>(i) - static_cast<float>(height) / 2.0f); // z-coordinate
            vertices_data.emplace_back(static_cast<float>(j) / width);  // U-texture coordinate
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

std::unique_ptr<IndexedMesh> create_indexed_grid_mesh(int width, int height, const Image<float>& height_map,
                                                      const CubicHermiteCurve& curve)
{
    auto grid_mesh_data = grid_mesh(width, height, height_map, curve);
    return std::make_unique<IndexedMesh>(std::move(grid_mesh_data.first), std::move(grid_mesh_data.second));
}

std::unique_ptr<PatchMesh> create_grid_patch(int width, int height, int number_of_patches)
{
    std::vector<float> vertices_data;
    const int number_of_attributes{5};
    const int vertices_per_patch{4};
    vertices_data.reserve(number_of_patches * number_of_patches * number_of_attributes * vertices_per_patch);

    const float half_width = static_cast<float>(width) / 2.0f;
    const float horizontal_ratio = static_cast<float>(width) / static_cast<float>(number_of_patches);
    const float half_height = static_cast<float>(height) / 2.0f;
    const float vertical_ratio = static_cast<float>(height) / static_cast<float>(number_of_patches);
    for (int x = 0; x < number_of_patches; ++x)
    {
        for (int z = 0; z < number_of_patches; ++z)
        {
            /*
            Note:
            A (input) quad with vertices [0, 1, 2, 3] is mapped to the
            abstract patch with vertices [p00, p10, p11, p01], respectively.
            Also note that when looking down the Y-axis, Z points down and X points
            right, while the parametric space has U pointing right and V pointing up.
            3--------2       p01------p11
            |        |        |        |
            |        |   ~    |        |
            |        |        |        |
            0--------1       p00------p10
            */
            vertices_data.emplace_back(x * horizontal_ratio - half_width);             // x-coordinate
            vertices_data.emplace_back(0.0f);                                          // y-coordinate
            vertices_data.emplace_back((z + 1) * vertical_ratio - half_height);        // z-coordinate
            vertices_data.emplace_back(static_cast<float>(x) / number_of_patches);     // u-coordinate
            vertices_data.emplace_back(static_cast<float>(z + 1) / number_of_patches); // v-coordinate

            vertices_data.emplace_back((x + 1) * horizontal_ratio - half_width);       // x-coordinate
            vertices_data.emplace_back(0.0f);                                          // y-coordinate
            vertices_data.emplace_back((z + 1) * vertical_ratio - half_height);        // z-coordinate
            vertices_data.emplace_back(static_cast<float>(x + 1) / number_of_patches); // u-coordinate
            vertices_data.emplace_back(static_cast<float>(z + 1) / number_of_patches); // v-coordinate

            vertices_data.emplace_back((x + 1) * horizontal_ratio - half_width);       // x-coordinate
            vertices_data.emplace_back(0.0f);                                          // y-coordinate
            vertices_data.emplace_back(z * vertical_ratio - half_height);              // z-coordinate
            vertices_data.emplace_back(static_cast<float>(x + 1) / number_of_patches); // u-coordinate
            vertices_data.emplace_back(static_cast<float>(z) / number_of_patches);     // v-coordinate

            vertices_data.emplace_back(x * horizontal_ratio - half_width);         // x-coordinate
            vertices_data.emplace_back(0.0f);                                      // y-coordinate
            vertices_data.emplace_back(z * vertical_ratio - half_height);          // z-coordinate
            vertices_data.emplace_back(static_cast<float>(x) / number_of_patches); // u-coordinate
            vertices_data.emplace_back(static_cast<float>(z) / number_of_patches); // v-coordinate
        }
    }

    return std::make_unique<PatchMesh>(vertices_per_patch, std::move(vertices_data));
}