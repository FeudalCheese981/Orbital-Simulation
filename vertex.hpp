#pragma once

#include <glm/glm.hpp>

// Stores information about a vertex
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec4 color;
	glm::vec2 textureUV;
};