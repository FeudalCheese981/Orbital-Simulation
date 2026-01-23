#include "triangleIcon.hpp"

TriangleIcon::TriangleIcon(glm::vec3 colour, std::string text, glm::vec3 pos)
	: Icon(colour, text, pos) // call the base class constructor
{
	// create VAO and VBO for the triangle
	glGenVertexArrays(1, &triangleVAO);
	glBindVertexArray(triangleVAO);
	glGenBuffers(1, &triangleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW); // single triangle only has 3 vertices
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0); // each vertex has x, y position
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

TriangleIcon::~TriangleIcon()
{
	// delete the VAO and VBO
	glDeleteBuffers(1, &triangleVBO);
	glDeleteVertexArrays(1, &triangleVAO);
}

void TriangleIcon::drawShape(Shader& shader, Camera& camera, glm::vec2 xyPos, glm::vec4 colour, float uiScale)
{
	// create variable to store vertices
	float vertices[3][2];
	// set size of triangle factoring in UI scale
	float size = 12 * uiScale;
	// set position of the triangle
	float x = xyPos.x; 
	float y = xyPos.y;
	// generate trinagle vertices
	vertices[0][0] = x;
	vertices[0][1] = y;
	vertices[1][0] = x + size / 2;
	vertices[1][1] = y + size;
	vertices[2][0] = x - size / 2;
	vertices[2][1] = y + size;

	// pass colour information to the shader
	shader.activate();
	glUniform4f(glGetUniformLocation(shader.getID(), "colour"), colour.x, colour.y, colour.z, colour.w);
	// bind buffer and vertex array and pass vertex data
	glBindVertexArray(triangleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	// unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// draw the triangle
	glDrawArrays(GL_TRIANGLES, 0, 3);
	// unbind the vertex array
	glBindVertexArray(0);
}