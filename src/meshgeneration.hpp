#ifndef MESH_GENERATION_HPP
#define MESH_GENERATION_HPP

#include <memory>

#include "image.hpp"
#include "mesh.hpp"

std::unique_ptr<IndexedMesh> grid_mesh(int width, int height);

#endif // MESH_GENERATION_HPP