#include "icon.hpp"

Icon::Icon(glm::vec4 color, std::string text, glm::vec3 pos)
{
	updateColor(color);
	updateText(text);
	updatePos(pos);

	glGenVertexArrays(1, &textVAO);
	glBindVertexArray(textVAO);
	glGenBuffers(1, &textVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Icon::~Icon()
{
	glDeleteBuffers(1, &textVBO);
	glDeleteVertexArrays(1, &textVAO);
}

void Icon::updateColor(glm::vec4 color)
{
	iconColor = color;
}

void Icon::updateText(std::string text)
{
	iconText = text;
}

void Icon::updatePos(glm::vec3 pos)
{
	iconPos = pos;
}

void Icon::draw(Shader& shapeShader, Shader& textShader, Camera& camera, Text& text)
{
	glm::vec4 pos = camera.orthogonalDisplay(iconPos);
	if (pos.w <= 0.0f)
		return;

	glm::vec2 xyPos = glm::vec2(pos.x, pos.y);

	glm::mat4 projection = camera.getOrthogonalProjection();
	shapeShader.activate();
	glUniformMatrix4fv(glGetUniformLocation(shapeShader.getID(), "projection"), 1, GL_FALSE, glm::value_ptr(camera.getOrthogonalProjection()));
	
	drawShape(shapeShader, camera, xyPos);
	//glDisable(GL_DEPTH_TEST);
	//drawText(textShader, camera, textLoader, xyPos);
	//glEnable(GL_DEPTH_TEST);
	text.draw(textShader, camera, iconText, xyPos, iconColor);
}

//void Icon::drawText(Shader& shader, Camera& camera, TextLoader& textLoader, glm::vec2 xyPos)
//{
//	//float x = xyPos.x + 16;
//	//float y = xyPos.y;
//	//shader.activate();
//	//glUniform1i(glGetUniformLocation(shader.getID(), "characters"), 0);
//	//glBindVertexArray(textVAO);
//
//	//std::string::const_iterator c;
//	//for (c = iconText.begin(); c < iconText.end(); c++)
//	//{
//	//	Character& ch = textLoader.getCharacter(*c);
//
//	//	//std::cout << ch.getSize().x << ", " << ch.getSize().y << "\n";
//	//	float xPos = x + ch.getBearing().x;
//	//	float yPos = y - (ch.getSize().y - ch.getBearing().y);
//
//	//	float w = ch.getSize().x;
//	//	float h = ch.getSize().y;
//
//	//	float vertices[6][4] = {
//	//		{ xPos,     yPos + h, 0.0f, 0.0f },
//	//		{ xPos,     yPos,     0.0f, 1.0f },
//	//		{ xPos + w, yPos,     1.0f, 1.0f },
//
//	//		{ xPos,     yPos + h, 0.0f, 0.0f },
//	//		{ xPos + w, yPos,     1.0f, 1.0f },
//	//		{ xPos + w, yPos + h, 1.0f, 0.0f }
//	//	};
//
//	//	ch.bind();
//	//	ch.characterUniform(shader, iconColor);
//	//	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
//	//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
//	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	//	glDrawArrays(GL_TRIANGLES, 0, 6);
//
//	//	x += (ch.getAdvance() >> 6);
//	//}
//	//glBindVertexArray(0);
//	//glBindTexture(GL_TEXTURE_2D, 0);
//
//	float x = xyPos.x + 16;
//	float y = xyPos.y;
//
//	shader.activate();
//	glUniform1i(glGetUniformLocation(shader.getID(), "characters"), 0);
//	glUniform4f(glGetUniformLocation(shader.getID(), "color"), iconColor.x, iconColor.y, iconColor.z, iconColor.w);
//	glActiveTexture(GL_TEXTURE0);
//	glBindVertexArray(textVAO);
//
//	std::string::const_iterator c;
//	for (c = iconText.begin(); c < iconText.end(); c++)
//	{
//		Character ch = textLoader.getCharacter(*c);
//	}
//}