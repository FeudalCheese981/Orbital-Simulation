#pragma once

#include <map>

#include "shader.hpp"
#include "camera.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

struct Character
{
	GLuint texture;
	glm::ivec2 size;
	glm::ivec2 bearing;
	unsigned int advance;
};

class Text
{
public:
	Text(int fontSize);
	~Text();

	void draw(Shader& shader, Camera& camera, std::string text, glm::ivec2 xyPos, glm::vec4 color, float uiScale);

private:
	std::map<unsigned char, Character> characters;

	GLuint VAO;
	GLuint VBO;
};