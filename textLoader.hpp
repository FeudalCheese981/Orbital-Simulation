#pragma once

#include <optional>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "character.hpp"

class TextLoader
{
public:
	TextLoader(int width, int height, int fontSize);
	~TextLoader();

	Character& getCharacter(unsigned char c);

private:
	std::map<unsigned char, Character> characters;
};