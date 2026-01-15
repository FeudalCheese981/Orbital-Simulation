#include "triangleIcon.hpp"

TriangleIcon::TriangleIcon(glm::vec3 colour, std::string text, glm::vec3 pos)
	: Icon(colour, text, pos)
{
	glGenVertexArrays(1, &triangleVAO);
	glBindVertexArray(triangleVAO);
	glGenBuffers(1, &triangleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

TriangleIcon::~TriangleIcon()
{
	glDeleteBuffers(1, &triangleVBO);
	glDeleteVertexArrays(1, &triangleVAO);
}

void TriangleIcon::drawShape(Shader& shader, Camera& camera, glm::vec2 xyPos, glm::vec4 colour, float uiScale)
{
	float vertices[3][2];
	float size = 12 * uiScale;

	float x = xyPos.x;
	float y = xyPos.y;

	vertices[0][0] = x;
	vertices[0][1] = y;
	
	vertices[1][0] = x + size / 2;
	vertices[1][1] = y + size;

	vertices[2][0] = x - size / 2;
	vertices[2][1] = y + size;

	shader.activate();
	glUniform4f(glGetUniformLocation(shader.getID(), "colour"), colour.x, colour.y, colour.z, colour.w);

	glBindVertexArray(triangleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}