#include "icon.hpp"

Icon::Icon(glm::vec3 colour, std::string text, glm::vec3 pos)
{
	// set the text, colour and position of the icon
	updateText(text);
	updateColor(colour);
	updatePos(pos);
}

void Icon::updateColor(glm::vec3 colour)
{
	iconColour = colour;
}

void Icon::updateText(std::string text)
{
	iconText = text;
}

void Icon::updatePos(glm::vec3 pos)
{
	iconPos = pos;
}

void Icon::draw(Shader& shapeShader, Shader& textShader, Camera& camera, Text& textObj, float uiScale)
{
	// get screen position of icon, via camera
	glm::vec4 pos = camera.orthogonalDisplay(iconPos);
	// if icon is behind camera don't draw
	if (pos.w <= 0.0f)
	{
		return;
	}

	// convert colour to RGBA
	glm::vec4 colour = glm::vec4(iconColour, 1.0f);
	// get camera position and distance scale
	glm::vec3 camPos = camera.getPos();
	glm::vec3 distanceScale = camera.getDistanceScale();
	
	// temporarily store the x and y co-ordinates of the icon
	glm::vec2 xyPos = glm::vec2(pos.x, pos.y);

	// get the orthogonal projection matrix and send to the shader
	glm::mat4 projection = camera.getOrthogonalProjection();
	shapeShader.activate();
	glUniformMatrix4fv(glGetUniformLocation(shapeShader.getID(), "projection"), 1, GL_FALSE, glm::value_ptr(camera.getOrthogonalProjection()));
	
	// draw the shape
	drawShape(shapeShader, camera, xyPos, colour, uiScale);
	// draw the text next to the shape
	textObj.draw(textShader, camera, iconText, xyPos + glm::vec2(16.0f, 0.0f), colour, uiScale);
}