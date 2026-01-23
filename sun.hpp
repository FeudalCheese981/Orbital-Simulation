#pragma once

#include "shape.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "transform.hpp"

// Sun class - Representing the sun in the solar system, as a reference frame
class Sun
{
public:
	Sun
	(
		glm::vec3 position,
		glm::quat rotation,
		glm::vec3 scale,
		double radius,
		double mass,
		glm::vec3 colour
	); // Initialise sun
	~Sun() = default;

	void sendLightInfoToShader(Shader& shader); // Passes information about light colour to a shader

	void draw(Shader& shader, Camera& camera); // Draws the sun

private:
	// Mesh and Transform for Sun
	Mesh sunMesh;
	Transform sunTransform;

	// Info about the sun
	double sunRadius;
	double sunMass;
	glm::vec3 sunPos;
	glm::vec4 sunColour;
};