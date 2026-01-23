#pragma once

#include "icon.hpp"

// CircleIcon class - icon with circle as shape
// inherits from Icon
class CircleIcon : public Icon
{
public:
	CircleIcon(glm::vec3 colour, std::string text, glm::vec3 pos); // load icon
	~CircleIcon();

	void drawShape(Shader& shader, Camera& camera, glm::vec2 xyPos, glm::vec4 colour, float uiScale) override; // draws the circle

private:
	// VAO and VBO id's for OpenGL
	GLuint circleVAO; 
	GLuint circleVBO;
};