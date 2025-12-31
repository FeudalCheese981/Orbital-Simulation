#include "mesh.hpp"

Mesh::Mesh(const MeshData& data)
{
	// Store new vertices and indices
	vertices = data.vertices;
	indices = data.indices;

	// Bind VAO to current context
	VAO.bind();
	VBO VBO(vertices); // Create temporart Vertex Buffer and Element Buffer Objects
	EBO EBO(indices);
	 
	// Link vertex attributes to Vertex Array Objects
	VAO.linkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)(0));
	VAO.linkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	VAO.linkAttrib(VBO, 2, 4, GL_FLOAT, sizeof(Vertex), (void*)(offsetof(Vertex, color)));
	VAO.linkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(offsetof(Vertex, textureUV)));

	// Unbind buffers/arrays
	VAO.unbind();
	VBO.unbind();
	EBO.unbind();
}

void Mesh::draw(GLenum type)
{
	VAO.bind();
	glDrawElements(type, indices.size(), GL_UNSIGNED_INT, 0);
}