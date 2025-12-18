#pragma once

#include "text.hpp"
#include "camera.hpp"

class Icon
{
public:
	virtual ~Icon();

	void updateColor(glm::vec4 color);
	void updateText(std::string text);
	void updatePos(glm::vec3 pos);

	void draw(Shader& shapeShader, Shader& textShader, Camera& camera, Text& text);

protected:
	Icon(glm::vec4 color, std::string text, glm::vec3 pos);

	virtual void drawShape(Shader& shader, Camera& camera, glm::vec2 xyPos) = 0;
	//void drawText(Shader& shader, Camera& camera, TextLoader& textLoader, glm::vec2 xyPos);

	GLuint textVAO;
	GLuint textVBO;
	
	glm::vec4 iconColor;
	std::string iconText;

	glm::vec3 iconPos;
};