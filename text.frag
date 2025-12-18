#version 460 core

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D characters;
uniform vec4 color;

void main()
{
	FragColor = vec4(1.0, 1.0, 1.0, texture(characters, texCoords).r) * color;
}