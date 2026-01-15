#pragma once

#include "icon.hpp"

class TriangleIcon : public Icon
{
public:
	TriangleIcon(glm::vec3 colour, std::string text, glm::vec3 pos);
	~TriangleIcon();

	void drawShape(Shader& shader, Camera& camera, glm::vec2 xyPos, glm::vec4 colour, float uiScale) override;

private:
	GLuint triangleVAO;
	GLuint triangleVBO;
};