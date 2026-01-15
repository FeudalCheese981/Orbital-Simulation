#version 460 core

out vec4 FragColour;

uniform vec4 colour;

void main()
{
	FragColour = colour;
}