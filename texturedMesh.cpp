#include "texturedMesh.hpp"

TexturedMesh::TexturedMesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
	: Mesh(vertices, indices)
{
	meshTextures = textures;
}

void TexturedMesh::draw(Shader& shader, Camera& camera, GLenum type)
{
	shader.activate();
	VAO.bind();

	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;
	unsigned int numNight = 0;
	for (unsigned int i = 0; i < meshTextures.size(); i++)
	{
		std::string num;
		std::string texType = meshTextures[i].getTexType();
		if (texType == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (texType == "specular")
		{
			num = std::to_string(numSpecular++);
		}
		else if (texType == "night")
		{
			num = std::to_string(numNight++);
		}
		meshTextures[i].bind();
		meshTextures[i].textureUnit(shader, (texType + num).c_str());
	}
	
	camera.cameraUniform(shader);
	glDrawElements(type, meshIndices.size(), GL_UNSIGNED_INT, 0);
}