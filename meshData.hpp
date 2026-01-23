#pragma once

#include "vertex.hpp"
#include <vector>

// Stores vertex and index data for meshes
struct MeshData
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};