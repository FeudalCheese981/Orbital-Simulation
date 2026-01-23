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
	: sunMesh(generateSphere(radius, 64, glm::vec4(colour, 1.0f))), // Generate the Mesh
	sunTransform(position, rotation, scale) // Initialise the Transform
{
	// set attributes
	sunRadius = radius;
	sunMass = mass;
	sunColour = glm::vec4(colour, 1.0f);
	sunPos = position;
}

void Sun::sendLightInfoToShader(Shader& shader)
{
	// Activate Shader
	shader.activate();
	// Send position information
	glUniform3f(glGetUniformLocation(shader.getID(), "lightPosition"), sunPos.x, sunPos.y, sunPos.z);
	// Send colour information
	glUniform4f(glGetUniformLocation(shader.getID(), "lightColour"), sunColour.x, sunColour.y, sunColour.z, sunColour.w);
}

void Sun::draw(Shader& shader, Camera& camera)
{
	// Activate Shader
	shader.activate();
	// Send Transformation Matrix To Shader
	sunTransform.uniform(shader);
	// Send Camera perspective matrix to Shader
	camera.cameraUniform(shader);
	// Draw the Mesh
	sunMesh.draw(GL_TRIANGLES);
}