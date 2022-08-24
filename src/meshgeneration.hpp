#ifndef MESH_GENERATION_HPP
#define MESH_GENERATION_HPP

#include <memory>

#include "image.hpp"
#include "mesh.hpp"

class CubicHermiteCurve;

std::unique_ptr<IndexedMesh> grid_mesh(int width, int height, const Image<float>& height_map, const CubicHermiteCurve& curve);

#endif // MESH_GENERATION_HPP