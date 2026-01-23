#pragma once

#include <map>

#include "shader.hpp"
#include "camera.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

// struct storing information about character from bitmap
struct Character
{
	GLuint texture; // texture ID
	glm::ivec2 size; // size of the bitmap
	glm::ivec2 bearing; // location of the bitmap
	unsigned int advance; // number of pixels to advance to get to next charater
};

// text class stores text for rendering
class Text
{
public:
	Text(int fontSize); // loads characters from bitmap
	~Text();

	void draw(Shader& shader, Camera& camera, std::string text, glm::ivec2 xyPos, glm::vec4 colour, float uiScale);

private:
	std::map<unsigned char, Character> characters; // stores characters in a map

	GLuint VAO; // VAO and VBO id's for OpenGL
	GLuint VBO;
};