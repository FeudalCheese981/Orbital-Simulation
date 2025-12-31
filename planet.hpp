#pragma once

#include "shape.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "transform.hpp"

class Planet
{
public:
	Planet
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
	);
	~Planet() = default;

	void rotateAboutAxis(float deltaTime, int dayLengthSeconds);

	void draw
	(
		Shader& planetShader,
		Shader& atmosphereShader,
		Camera& camera
	);

	glm::vec3 getPos();
	glm::quat getRotation();
	std::string getName();
	double getMass();
	double getRadius();


private:
	Texture diffuseTexture;
	Texture specularTexture;
	Texture nightTexture;

	Mesh planetMesh;
	Mesh atmosphereMesh;

	Transform planetTransform;

	std::string planetName;

	glm::vec3 planetPosition;
	glm::quat planetRotation;

	double planetRadius;
	double planetAtmosphereHeight;
	double planetMass;
};