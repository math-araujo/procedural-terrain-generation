#ifndef MESH_GENERATION_HPP
#define MESH_GENERATION_HPP

#include <memory>
#include <utility>
#include <vector>

#include "image.hpp"

class CubicHermiteCurve;
class IndexedMesh;

std::pair<std::vector<float>, std::vector<std::uint32_t>> grid_mesh(int width, int height, const Image<float>& height_map, const CubicHermiteCurve& curve);

std::unique_ptr<IndexedMesh> create_indexed_grid_mesh(int width, int height, const Image<float>& height_map, const CubicHermiteCurve& curve);

#endif // MESH_GENERATION_HPP