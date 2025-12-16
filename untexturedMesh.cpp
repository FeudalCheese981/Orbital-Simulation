#include "untexturedMesh.hpp"

UntexturedMesh::UntexturedMesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices)
	: Mesh(vertices, indices) 
{}

void UntexturedMesh::draw(Shader& shader, Camera& camera, GLenum type)
{
	shader.activate();
	VAO.bind();
	camera.cameraUniform(shader);
	glDrawElements(type, meshIndices.size(), GL_UNSIGNED_INT, 0);

}