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
	: planetMesh(generateSphere(radius, 1024, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))),
	atmosphereMesh(generateSphere(radius + atmosphereHeight, 1024, glm::vec4(atmosphereColour, 0.5f))),
	diffuseTexture(diffuseFile, "diffuse", 1),
	specularTexture(specularFile, "specular", 2),
	nightTexture(nightFile, "night", 3),
	planetTransform(position, rotation, scale)
{
	planetPosition = position;
	planetRotation = rotation;

	planetName = name;
	planetRadius = radius;
	planetAtmosphereHeight = atmosphereHeight;
	planetMass = mass;
}

void Planet::rotateAboutAxis(float deltaTime, int dayLengthSeconds)
{
	glm::vec3 axis = glm::normalize(glm::rotate(planetRotation, glm::vec3(0.0f, 0.0f, 1.0f)));
	float angle = glm::radians(360 * (deltaTime / (float)dayLengthSeconds));
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
	planetShader.activate();
	planetTransform.uniform(planetShader);
	diffuseTexture.bind();
	diffuseTexture.textureUniform(planetShader, "diffuse0");
	specularTexture.bind();
	specularTexture.textureUniform(planetShader, "specular0");
	nightTexture.bind();
	nightTexture.textureUniform(planetShader, "night0");
	camera.cameraUniform(planetShader);
	planetMesh.draw(GL_TRIANGLES);

	glDisable(GL_DEPTH_TEST);
	atmosphereShader.activate();
	planetTransform.uniform(atmosphereShader);
	camera.cameraUniform(atmosphereShader);
	atmosphereMesh.draw(GL_TRIANGLES);
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