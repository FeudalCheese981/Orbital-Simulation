#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec2 aTexUV;

out vec3 crntPos;
out vec4 color;

uniform mat4 cameraMatrix;

void main()
{
	crntPos = aPos;
	color = aColor;

	gl_Position = cameraMatrix * vec4(crntPos, 1.0);
}