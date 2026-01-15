#define _USE_MATH_DEFINES
#include <cmath>

#include "circleIcon.hpp"

CircleIcon::CircleIcon(glm::vec3 colour, std::string text, glm::vec3 pos)
	: Icon(colour, text, pos)
{
	glGenVertexArrays(1, &circleVAO);
	glBindVertexArray(circleVAO);
	glGenBuffers(1, &circleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
	glBufferData(GL_ARRAY_BUFFER, 48 * 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

CircleIcon::~CircleIcon()
{
	glDeleteBuffers(1, &circleVBO);
	glDeleteVertexArrays(1, &circleVAO);
}

void CircleIcon::drawShape(Shader& shader, Camera& camera, glm::vec2 xyPos, glm::vec4 colour, float uiScale)
{
	float vertices[48][2];
	float r = 6 * uiScale;

	int v = 0;
	for (int i = 0; i < 16; i++)
	{
		float theta1 = 2.0f * M_PI * (float)i / 16.0f;
		float theta2 = 2.0f * M_PI * (float)(i + 1) / 16.0f;
		float x1 = r * cos(theta1) + xyPos.x;
		float y1 = r * sin(theta1) + xyPos.y;

		float x2 = r * cos(theta2) + xyPos.x;
		float y2 = r * sin(theta2) + xyPos.y;


		vertices[v][0] = xyPos.x;
		vertices[v][1] = xyPos.y;
		v++;
		vertices[v][0] = x1;
		vertices[v][1] = y1;
		v++;
		vertices[v][0] = x2;
		vertices[v][1] = y2;
		v++;
	}

	shader.activate();
	glUniform4f(glGetUniformLocation(shader.getID(), "colour"), colour.x, colour.y, colour.z, colour.w);

	glBindVertexArray(circleVAO);

	glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_TRIANGLES, 0, 48);

	glBindVertexArray(0);
}