#pragma once

#include "icon.hpp"

// TriangleIcon class - icon with triangle as shape
class TriangleIcon : public Icon
{
public:
	TriangleIcon(glm::vec3 colour, std::string text, glm::vec3 pos); // load icon
	~TriangleIcon();

	void drawShape(Shader& shader, Camera& camera, glm::vec2 xyPos, glm::vec4 colour, float uiScale) override; // draws the triangle

private:
	// VAO and VBO id's for OpenGL
	GLuint triangleVAO;
	GLuint triangleVBO;
};