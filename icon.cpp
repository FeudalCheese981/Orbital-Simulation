#include "icon.hpp"

Icon::Icon(glm::vec4 color, std::string text)
{
	updateColor(color);
	updateText(text);

	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
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

void Icon::drawText(Shader& shader, Camera& camera, TextLoader& textLoader)
{
	glm::vec4 pos = camera.orthogonalDisplay();
	if (pos.w <= 0.0f)
		return;
	
	float x = pos.x + 12;
	float y = pos.y;

	shader.activate();
	glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "projection"), 1, GL_FALSE, glm::value_ptr(camera.getOrthogonalProjection()));

	std::string::const_iterator c;
	for (c = iconText.begin(); c < iconText.end(); c++)
	{
		Character ch = textLoader.getCharacter(*c);

		float xPos = x * ch.getBearing().x;
		float yPos = y - (ch.getSize().y - ch.getBearing().y);

		float w = ch.getSize().x;
		float h = ch.getSize().y;

		float vertices[6][4] = {
			{ xPos,     yPos + h, 0.0f, 0.0f },
			{ xPos,     yPos,     0.0f, 1.0f },
			{ xPos + w, yPos,     1.0f, 1.0f },

			{ xPos,     yPos + h, 0.0f, 0.0f },
			{ xPos + w, yPos,     1.0f, 1.0f },
			{ xPos + w, yPos + h, 1.0f, 0.0f }
		};

		ch.bind();
		glBindBuffer(GL_ARRAY_BUFFER, ch.getID());
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.getAdvance() >> 6);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}