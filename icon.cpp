#include "icon.hpp"

Icon::Icon(glm::vec3 color, std::string text, glm::vec3 pos)
{
	updateColor(color);
	updateText(text);
	updatePos(pos);

	glGenVertexArrays(1, &textVAO);
	glBindVertexArray(textVAO);
	glGenBuffers(1, &textVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Icon::~Icon()
{
	glDeleteBuffers(1, &textVBO);
	glDeleteVertexArrays(1, &textVAO);
}

void Icon::updateColor(glm::vec3 color)
{
	iconColor = color;
}

void Icon::updateText(std::string text)
{
	iconText = text;
}

void Icon::updatePos(glm::vec3 pos)
{
	iconPos = pos;
}

void Icon::draw(Shader& shapeShader, Shader& textShader, Camera& camera, Text& textObj)
{
	glDisable(GL_DEPTH_TEST);
	glm::vec4 pos = camera.orthogonalDisplay(iconPos);
	if (pos.w <= 0.0f)
	{
		return;
	}

	glm::vec4 color = glm::vec4(iconColor, 1.0f);
	glm::vec3 camPos = camera.getPos();
	glm::vec3 distanceScale = camera.getDistanceScale();
	
	glm::vec2 xyPos = glm::vec2(pos.x, pos.y);

	glm::mat4 projection = camera.getOrthogonalProjection();
	shapeShader.activate();
	glUniformMatrix4fv(glGetUniformLocation(shapeShader.getID(), "projection"), 1, GL_FALSE, glm::value_ptr(camera.getOrthogonalProjection()));
	
	drawShape(shapeShader, camera, xyPos, color);
	glEnable(GL_DEPTH_TEST);

	textObj.draw(textShader, camera, iconText, xyPos, color);
}

// fades out text if too far away:
//float distance = abs(glm::distance(camPos, iconPos * distanceScale));
//if (distance >= 3.0f)
//	color.w = 1.3f - (distance / 10.0f);
//else
//	color.w = 1.0f;