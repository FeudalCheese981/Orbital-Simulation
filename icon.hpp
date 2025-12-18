#pragma once

#include "text.hpp"
#include "camera.hpp"

class Icon
{
public:
	virtual ~Icon();

	void updateColor(glm::vec3 color);
	void updateText(std::string text);
	void updatePos(glm::vec3 pos);

	void draw(Shader& shapeShader, Shader& textShader, Camera& camera, Text& text);

protected:
	Icon(glm::vec3 color, std::string text, glm::vec3 pos);

	virtual void drawShape(Shader& shader, Camera& camera, glm::vec2 xyPos, glm::vec4 color) = 0;
	
	GLuint textVAO;
	GLuint textVBO;
	
	glm::vec3 iconColor;
	std::string iconText;

	glm::vec3 iconPos;
};