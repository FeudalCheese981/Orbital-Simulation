#pragma once

#include "mesh.hpp"
#include "texture.hpp"

class TexturedMesh : public Mesh
{
public:
	TexturedMesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures);
	~TexturedMesh() = default;

	void draw(Shader& shader, Camera& camera, GLenum type) override;

private:
	std::vector<Texture> meshTextures;
};