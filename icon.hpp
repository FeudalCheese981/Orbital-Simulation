#pragma once

#include "textLoader.hpp"
#include "camera.hpp"

class Icon
{
public:
	virtual ~Icon();

	void draw(Shader& shader, Camera& camera, TextLoader& textLoader);

	void updateColor(glm::vec4 color);
	void updateText(std::string text);

protected:
	Icon(glm::vec4 color, std::string text);

	virtual void drawShape(Shader& shader, Camera& camera) = 0;
	void drawText(Shader& shader, Camera& camera, TextLoader& textLoader);

	GLuint textVAO;
	GLuint textVBO;
	
	glm::vec4 iconColor;
	std::string iconText;
};