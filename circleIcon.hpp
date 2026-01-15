#pragma once

#include "icon.hpp"

class CircleIcon : public Icon
{
public:
	CircleIcon(glm::vec3 colour, std::string text, glm::vec3 pos);
	~CircleIcon();

	void drawShape(Shader& shader, Camera& camera, glm::vec2 xyPos, glm::vec4 colour, float uiScale) override;

private:
	GLuint circleVAO;
	GLuint circleVBO;
};