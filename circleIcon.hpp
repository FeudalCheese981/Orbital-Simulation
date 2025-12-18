#pragma once

#include "icon.hpp"

class CircleIcon : public Icon
{
public:
	CircleIcon(glm::vec4 color, std::string text, glm::vec3 pos);
	~CircleIcon();

	void drawShape(Shader& shader, Camera& camera, glm::vec2 xyPos) override;

private:
	GLuint circleVAO;
	GLuint circleVBO;
};