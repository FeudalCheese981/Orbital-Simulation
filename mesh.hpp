#pragma once

#include "VAO.hpp"
#include "EBO.hpp"
#include "shader.hpp"
#include "camera.hpp"

class Mesh
{
public:
	virtual ~Mesh() = default;
	
	virtual void draw(Shader& shader, Camera& camera, GLenum type) = 0;

protected:
	Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);

	VAO VAO;
	std::vector<Vertex> meshVertices;
	std::vector<GLuint> meshIndices;
};