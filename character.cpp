#include "character.hpp"

Character::Character(FT_Face& face, unsigned char c)
{
	if (FT_Load_Char(face, c, FT_LOAD_RENDER))
	{
		std::cerr << "Failed to load glyph: '" << c << "'\n";
		return;
	}

	glGenTextures(1, &ID);
	bind();
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
	bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
	advance = face->glyph->advance.x;
}

Character::~Character()
{
	glDeleteTextures(1, &ID);
}

void Character::characterUniform(Shader& shader, glm::vec4 color)
{
	shader.activate();
	glUniform1i(glGetUniformLocation(shader.getID(), "characters"), unit);
	glUniform4f(glGetUniformLocation(shader.getID(), "color"), color.x, color.y, color.z, color.w);
}

void Character::bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Character::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Character::getID()
{
	return ID;
}

glm::ivec2 Character::getSize()
{
	return size;
}

glm::ivec2 Character::getBearing()
{
	return bearing;
}

unsigned int Character::getAdvance()
{
	return advance;
}