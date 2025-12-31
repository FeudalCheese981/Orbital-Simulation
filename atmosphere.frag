#version 460 core

in vec3 crntPos;
in vec3 normal;
in vec4 colour;
in vec2 textureUV;

out vec4 FragColour;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform vec4 lightColour;

void main()
{
	vec3 norm = normalize(normal);
	vec3 lightDirection = normalize(lightPosition - crntPos);

	float ambient = 0.01f;
	float diffuse = max(dot(norm, lightDirection), 0.0f);
	float specularLight = 0.5f;
	vec3 viewDirection = normalize(cameraPosition - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, norm);
	
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 2);
	float specular = specAmount * specularLight * diffuse;

	FragColour = vec4(colour.xyz * (diffuse + specular + ambient), colour.w);
}