#pragma once

#include "mesh.hpp"

class UntexturedMesh : public Mesh
{
public:
	UntexturedMesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
	~UntexturedMesh() = default;

	void draw(Shader& shader, Camera& camera, GLenum type) override;
};