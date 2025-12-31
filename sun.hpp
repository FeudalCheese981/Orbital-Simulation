#pragma once

#include "shape.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "transform.hpp"

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
	);
	~Sun() = default;

	void sendLightInfoToShader(Shader& shader);

	void draw(Shader& shader, Camera& camera);

private:
	Mesh sunMesh;
	Transform sunTransform;

	double sunRadius;
	double sunMass;
	glm::vec3 sunPos;
	glm::vec4 sunColour;
};