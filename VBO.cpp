#include "VBO.hpp"

VBO::VBO(std::vector<Vertex> vertices)
{
	glGenBuffers(1, &ID);
	bind();
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW); // pass data to buffer
	//unbind();
}

VBO::~VBO()
{
	glDeleteBuffers(1, &ID);
}

void VBO::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}