#version 460 core

in vec3 crntPos;
in vec3 normal;
in vec4 colour;
in vec2 textureUV;

out vec4 FragColour;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D night0;

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
	
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
	float specular = specAmount * specularLight * diffuse;

	float nightAmbient = clamp(0.3 - diffuse, 0.0, 1.0);

	FragColour = vec4(vec3((texture(diffuse0, textureUV) * (diffuse + ambient) 
					  + texture(specular0, textureUV).r * specular
	 				  + texture(night0, textureUV) * nightAmbient) * lightColour), 1.0);
}