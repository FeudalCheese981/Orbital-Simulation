#pragma once

#include "vertex.hpp"
#include <vector>

struct MeshData
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};