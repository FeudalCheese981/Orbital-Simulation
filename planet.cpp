#include "planet.hpp"

Planet::Planet
(
	std::string name,
	glm::vec3 position,
	glm::quat rotation,
	glm::vec3 scale,
	double radius,
	double atmosphereHeight,
	double mass,
	const char* diffuseFile,
	const char* specularFile,
	const char* nightFile,
	glm::vec3 atmosphereColour
)
	: planetMesh(generateSphere(radius, 1024, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))), // Initialise Meshes
	atmosphereMesh(generateSphere(radius + atmosphereHeight, 1024, glm::vec4(atmosphereColour, 0.5f))),
	diffuseTexture(diffuseFile, "diffuse", 1), // Initialise Textures 
	specularTexture(specularFile, "specular", 2),
	nightTexture(nightFile, "night", 3),
	planetTransform(position, rotation, scale) // Initialise Transform
{
	// Set Position and Rotation
	planetPosition = position;
	planetRotation = rotation;

	// Set Attributes
	planetName = name;
	planetRadius = radius;
	planetAtmosphereHeight = atmosphereHeight;
	planetMass = mass;
}

void Planet::rotateAboutAxis(float deltaTime, int dayLengthSeconds)
{
	// Compute the planet axis from the rotation
	glm::vec3 axis = glm::normalize(glm::rotate(planetRotation, glm::vec3(0.0f, 0.0f, 1.0f)));
	// Compute the angle of rotation based on the change in time, and the length of the planet day
	float angle = glm::radians(360 * (deltaTime / (float)dayLengthSeconds));
	// Compute the new Rotation
	glm::quat newRotation = glm::normalize(glm::angleAxis(angle, axis));
	planetTransform.setRotation(newRotation * glm::normalize(glm::quat_cast(planetTransform.getRotationMatrix())));
	planetRotation = glm::normalize(glm::quat_cast(planetTransform.getRotationMatrix()));
}

void Planet::draw
(
	Shader& planetShader,
	Shader& atmosphereShader,
	Camera& camera
)
{
	// Activate Shader for Surface
	planetShader.activate();
	// Send Transformation matrix to Shader
	planetTransform.uniform(planetShader); 
	// Bind Textures and send to Shader
	diffuseTexture.bind();
	diffuseTexture.textureUniform(planetShader, "diffuse0");
	specularTexture.bind();
	specularTexture.textureUniform(planetShader, "specular0");
	nightTexture.bind();
	nightTexture.textureUniform(planetShader, "night0");
	// Send Camera perspective Matrix to shader
	camera.cameraUniform(planetShader);
	// Draw the Surface Mesh
	planetMesh.draw(GL_TRIANGLES);

	// Disable Depth Testing so transparent atmosphere displays properly
	glDisable(GL_DEPTH_TEST);
	// Activate Shader for atmosphere
	atmosphereShader.activate();
	// Send Transformation matrix to Shader
	planetTransform.uniform(atmosphereShader);
	// Send Camera Perspective matrix to shader
	camera.cameraUniform(atmosphereShader);
	// Draw the Atmosphere Mesh
	atmosphereMesh.draw(GL_TRIANGLES);
	// Re-enable Depth Testing
	glEnable(GL_DEPTH_TEST);
}

glm::vec3 Planet::getPos()
{
	return planetPosition;
}

glm::quat Planet::getRotation()
{
	return planetRotation;
}

std::string Planet::getName()
{
	return planetName;
}

double Planet::getMass()
{
	return planetMass;
}

double Planet::getRadius()
{
	return planetRadius;
}

void Planet::updatePos(glm::vec3 pos)
{
	planetPosition = pos;
	planetTransform.setPosition(pos);
}