#include "sun.hpp"

Sun::Sun
(
	glm::vec3 position,
	glm::quat rotation,
	glm::vec3 scale,
	double radius,
	double mass,
	glm::vec3 colour
)
	: sunMesh(generateSphere(radius, 64, glm::vec4(colour, 1.0f))),
	sunTransform(position, rotation, scale)
{
	sunRadius = radius;
	sunMass = mass;
	sunColour = glm::vec4(colour, 1.0f);
	sunPos = position;
}

void Sun::sendLightInfoToShader(Shader& shader)
{
	shader.activate();
	glUniform3f(glGetUniformLocation(shader.getID(), "lightPosition"), sunPos.x, sunPos.y, sunPos.z);
	glUniform4f(glGetUniformLocation(shader.getID(), "lightColour"), sunColour.x, sunColour.y, sunColour.z, sunColour.w);
}

void Sun::draw(Shader& shader, Camera& camera)
{
	shader.activate();
	sunTransform.uniform(shader);
	camera.cameraUniform(shader);
	sunMesh.draw(GL_TRIANGLES);
}