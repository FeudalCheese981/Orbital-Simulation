#include "texture.hpp"

Texture::Texture(const char* image, const char* texType, GLuint slot)
{
	type = texType;
	unit = slot;

	int imgWidth, imgHeight, numColCh;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* imgData = stbi_load(image, &imgWidth, &imgHeight, &numColCh, 0); // Get image data

	glGenTextures(1, &ID);
	bind();

	// set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// Set number of color channels that texture uses
	switch (numColCh)
	{
	case 4:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
		break;
	case 3:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
		break;
	case 1:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RED, GL_UNSIGNED_BYTE, imgData);
		break;
	default:
		throw std::runtime_error("Could not recognize texture color format!");
		break;
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(imgData); // free image data
	unbind();
}

Texture::~Texture()
{
	glDeleteTextures(1, &ID);
}

void Texture::textureUniform(Shader& shader, const char* uniform)
{
	glUniform1i(glGetUniformLocation(shader.getID(), uniform), unit); // send uniform
}

const char* Texture::getTexType()
{
	return type;
}

void Texture::bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}