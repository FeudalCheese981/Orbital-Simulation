#version 460 core

in vec2 texCoords;
out vec4 FragColour;

uniform sampler2D characters;
uniform vec4 colour;

void main()
{
	FragColour = vec4(1.0, 1.0, 1.0, texture(characters, texCoords).r) * colour;
}