#pragma once

#include "text.hpp"
#include "camera.hpp"

class Icon
{
public:
	virtual ~Icon();

	void updateColor(glm::vec3 colour);
	void updateText(std::string text);
	void updatePos(glm::vec3 pos);

	void draw(Shader& shapeShader, Shader& textShader, Camera& camera, Text& textObj, float uiScale);

protected:
	Icon(glm::vec3 colour, std::string text, glm::vec3 pos);

	virtual void drawShape(Shader& shader, Camera& camera, glm::vec2 xyPos, glm::vec4 colour, float uiScale) = 0;
	
	GLuint textVAO;
	GLuint textVBO;
	
	glm::vec3 iconColour;
	std::string iconText;
	glm::vec3 iconPos;
};