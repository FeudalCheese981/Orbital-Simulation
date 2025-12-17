#include "textLoader.hpp"

TextLoader::TextLoader(int width, int height, int fontSize)
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
		Character character(face, c);
		characters.insert(std::pair<unsigned char, Character>(c, character));
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

TextLoader::~TextLoader()
{
	characters.clear();
}

Character& TextLoader::getCharacter(unsigned char c)
{
	return characters.at(c);
}