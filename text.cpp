#include "text.hpp"

Text::Text(int fontSize)
{
	FT_Library ft; // create library and face variables
	FT_Face face;

	if (FT_Init_FreeType(&ft)) // load the freetype library
	{
		std::cerr << "ERROR::FREETYPE: Failed to init FreeType Library!\n";
		return;
	}
	if (FT_New_Face(ft, "fonts/DejaVuSans.ttf", 0, &face)) // load the font
	{
		std::cerr << "ERROR::FREETYPE: Failed to load font!\n";
		return;
	}

	FT_Set_Pixel_Sizes(face, 0, fontSize); // set the font size

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // change the unpack alingment as bitmap stored with single chanel (8-bit)
	for (unsigned char c = 0; c < 128; c++) // load all ASCII characters
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) // error checking if glyph cant be loaded for some reason
		{
			std::cerr << "ERROR::FREETYPE: Failed to load Glyph!" << std::endl;
			continue; // skip to next glyph
		}
		GLuint texture; // create texture
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // set texture mipmap and wraping 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// set actual texture parameters
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

		glGenerateMipmap(GL_TEXTURE_2D); // generate mipmaps

		// store glyphs in the map
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		characters.insert(std::pair<char, Character>(c, character));
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // reset unpack alignmnent

	FT_Done_Face(face); // free memory of freetype library
	FT_Done_FreeType(ft);

	// setup VAO and VBO for texture Quad, spe
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW); // dynamic draw is used as single quad is used to render any character
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Text::~Text()
{
	for (int i = 0; i < characters.size(); i++) // clear all character textures from the map
	{
		glDeleteTextures(1, &characters[i].texture);
	}
	characters.clear();
	glDeleteBuffers(1, &VBO); // delete the buffers freeing up the memory
	glDeleteVertexArrays(1, &VAO);
}

void Text::draw(Shader& shader, Camera& camera, std::string text, glm::ivec2 xyPos, glm::vec4 colour, float uiScale)
{
	glDisable(GL_DEPTH_TEST); // disable depth testing as the 
	glm::mat4 projection = camera.getOrthogonalProjection(); // get the orthogonal projection matrix from the camera

	float x = xyPos.x; // set x and y position
	float y = xyPos.y;

	shader.activate(); // activate the shader
	// pass projection matrix, the text texture and the text colour to the shader
	glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1i(glGetUniformLocation(shader.getID(), "text"), 0);
	glUniform4f(glGetUniformLocation(shader.getID(), "colour"), colour.x, colour.y, colour.z, colour.w);
	// set the active texture and bind the vertex array
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	
	std::string::const_iterator c;
	for (c = text.begin(); c < text.end(); c++) // iterate through the text 
	{
		Character ch = characters[*c]; // get the specific character

		float xPos = x + ch.bearing.x * uiScale; // set the position, taking into account the scale of the UI
		float yPos = y - (ch.size.y - ch.bearing.y) * uiScale;

		float w = ch.size.x * uiScale; // set the size of the character, taking into account the scale of the UI
		float h = ch.size.y * uiScale;

		float vertices[6][4] = {
			{ xPos,     yPos + h, 0.0f, 0.0f },
			{ xPos,     yPos,     0.0f, 1.0f },
			{ xPos + w, yPos,     1.0f, 1.0f },

			{ xPos,     yPos + h, 0.0f, 0.0f },
			{ xPos + w, yPos,     1.0f, 1.0f },
			{ xPos + w, yPos + h, 1.0f, 0.0f }
		}; // create the vertices for the quad to render text onto

		glBindTexture(GL_TEXTURE_2D, ch.texture); // bind texture/buffer and pass vertex data to GPU
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer

		glDrawArrays(GL_TRIANGLES, 0, 6); // draw the quad

		x += (ch.advance >> 6) * uiScale; // advance by number of pixels to x pos for next character
	}
	glBindVertexArray(0); // unbind the vertex array and texture
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST); // re-enable depth testing
}