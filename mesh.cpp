#include "mesh.hpp"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices)
{
	// Store new vertices and indices
	meshVertices = vertices;
	meshIndices = indices;

	// Bind VAO to current context
	VAO.bind();
	VBO VBO(meshVertices); // Create temporart Vertex Buffer and Element Buffer Objects
	EBO EBO(meshIndices);
	 
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