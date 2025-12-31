#pragma once

#include "shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

class Transform
{
public:
	Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale);
	~Transform() = default;

	void setPosition(glm::vec3 position);
	void setRotation(glm::quat rotation);
	void setScale(glm::vec3 scale);
	
	void uniform(Shader& shader);

	const glm::mat4 getTranslationMatrix();
	const glm::mat4 getRotationMatrix();
	const glm::mat4 getScaleMatrix();

private:
	glm::mat4 translationMatrix;
	glm::mat4 rotationMatrix;
	glm::mat4 scaleMatrix;
};