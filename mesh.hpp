#pragma once

#include "EBO.hpp"
#include "VAO.hpp"
#include "meshData.hpp"

class Mesh
{
public:
	Mesh(const MeshData& data);
	~Mesh() = default;
	
	void draw(GLenum type);

private:
	VAO VAO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};