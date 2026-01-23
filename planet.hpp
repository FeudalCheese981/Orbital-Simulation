#pragma once

#include "shape.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "transform.hpp"

// Planet Class - stores information about a planet
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
	); // Initialise Planet with Textures
	~Planet() = default;

	void rotateAboutAxis(float deltaTime, int dayLengthSeconds); // Rotate the Planet around its axis of rotation

	void draw
	(
		Shader& planetShader,
		Shader& atmosphereShader,
		Camera& camera
	); // Draw the Planet in the scene

	// Getters for Planet Attributes
	glm::vec3 getPos();
	glm::quat getRotation();
	std::string getName();
	double getMass();
	double getRadius();

	void updatePos(glm::vec3 pos); // Set new Position for planet

private:
	// Planet Textures
	Texture diffuseTexture;
	Texture specularTexture;
	Texture nightTexture;

	// Planet Meshes
	Mesh planetMesh;
	Mesh atmosphereMesh;

	// Planet Transform
	Transform planetTransform;
	
	// Raw Position and Rotation Information
	glm::vec3 planetPosition;
	glm::quat planetRotation;

	// Other Planet Attributes
	std::string planetName;
	double planetRadius;
	double planetAtmosphereHeight;
	double planetMass;
};