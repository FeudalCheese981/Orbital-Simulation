#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColour;
layout (location = 3) in vec2 aTexUV;

out vec3 crntPos;
out vec3 normal;
out vec4 colour;
out vec2 textureUV;

uniform mat4 cameraMatrix;
uniform mat4 distanceScale;
uniform mat4 model;

uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
	crntPos = vec3(translation * rotation * scale * vec4(aPos, 1.0));
	normal = vec3(rotation * scale * vec4(aNormal, 1.0));
	colour = aColour;
	textureUV = aTexUV;

	gl_Position = cameraMatrix * distanceScale * vec4(crntPos, 1.0);
}