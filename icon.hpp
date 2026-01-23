#pragma once

#include "text.hpp"
#include "camera.hpp"

// Icon class for drawing icons (shape + text) onto screen
// This class is an abstract base class
class Icon
{
public:
	virtual ~Icon() = default;

	void updateColor(glm::vec3 colour); // 
	void updateText(std::string text);  // update icon info
	void updatePos(glm::vec3 pos);		//

	void draw(Shader& shapeShader, Shader& textShader, Camera& camera, Text& textObj, float uiScale); // draw the icon

protected:
	Icon(glm::vec3 colour, std::string text, glm::vec3 pos); // load the icon with parameters
	// pure virtual function must be implented in derived classes
	virtual void drawShape(Shader& shader, Camera& camera, glm::vec2 xyPos, glm::vec4 colour, float uiScale) = 0; 
	
	// store of the icon's colour, position
	glm::vec3 iconColour; 
	std::string iconText;
	glm::vec3 iconPos;
};