#pragma once

#include "EBO.hpp"
#include "VAO.hpp"
#include "meshData.hpp"

// Mesh Class - stores vertices for objects and correctly draws them
class Mesh
{
public:
	Mesh(const MeshData& data); // Initialise The Mesh with input vertex data
	~Mesh() = default;
	
	void draw(GLenum type); // Draw the Mesh

private:
	// Mesh stores a Vertex Array Object
	VAO VAO;
	// Vertex and Indices stored
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};