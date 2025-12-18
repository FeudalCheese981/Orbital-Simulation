#pragma once

#include "shader.hpp"
#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

class Character
{
public:
	Character(FT_Face& face, unsigned char c);
	~Character();

	void characterUniform(Shader& shader, glm::vec4 color);
	void bind();
	void unbind();

	GLuint getID();
	glm::ivec2 getSize();
	glm::ivec2 getBearing();
	unsigned int getAdvance();

private:
	GLuint ID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	unsigned int advance;

	unsigned char character;
};