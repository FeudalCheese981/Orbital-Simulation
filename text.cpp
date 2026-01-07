#include "text.hpp"

Text::Text(int fontSize)
{
	FT_Library ft;
	FT_Face face;

	if (FT_Init_FreeType(&ft))
	{
		std::cerr << "ERROR::FREETYPE: Failed to init FreeType Library!\n";
		return;
	}
	if (FT_New_Face(ft, "fonts/DejaVuSans.ttf", 0, &face))
	{
		std::cerr << "ERROR::FREETYPE: Failed to load font!\n";
		return;
	}

	FT_Set_Pixel_Sizes(face, 0, fontSize);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cerr << "ERROR::FREETYPE: Failed to load Glyph!" << std::endl;
			continue;
		}
		GLuint texture; // create texture
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		glGenerateMipmap(GL_TEXTURE_2D);
		// store glyphs in a map
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		characters.insert(std::pair<char, Character>(c, character));
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Text::~Text()
{
	for (int i = 0; i < characters.size(); i++)
	{
		glDeleteTextures(1, &characters[i].texture);
	}
	characters.clear();
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void Text::draw(Shader& shader, Camera& camera, std::string text, glm::ivec2 xyPos, glm::vec4 color, float uiScale)
{
	glDisable(GL_DEPTH_TEST);
	glm::mat4 projection = camera.getOrthogonalProjection();

	float x = xyPos.x + 16.0;
	float y = xyPos.y;

	shader.activate();
	glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1i(glGetUniformLocation(shader.getID(), "text"), 0);
	glUniform4f(glGetUniformLocation(shader.getID(), "color"), color.x, color.y, color.z, color.w);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	
	std::string::const_iterator c;
	for (c = text.begin(); c < text.end(); c++)
	{
		Character ch = characters[*c];

		float xPos = x + ch.bearing.x * uiScale;
		float yPos = y - (ch.size.y - ch.bearing.y) * uiScale;

		float w = ch.size.x * uiScale;
		float h = ch.size.y * uiScale;

		float vertices[6][4] = {
			{ xPos,     yPos + h, 0.0f, 0.0f },
			{ xPos,     yPos,     0.0f, 1.0f },
			{ xPos + w, yPos,     1.0f, 1.0f },

			{ xPos,     yPos + h, 0.0f, 0.0f },
			{ xPos + w, yPos,     1.0f, 1.0f },
			{ xPos + w, yPos + h, 1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, ch.texture);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.advance >> 6) * uiScale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST);
}