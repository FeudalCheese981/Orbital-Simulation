#include "transform.hpp"

Transform::Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale)
{
	setPosition(position);
	setRotation(rotation);
	setScale(scale);
}

void Transform::setPosition(glm::vec3 position)
{
	translationMatrix = glm::translate(glm::mat4(1.0f), position);
}

void Transform::setRotation(glm::quat rotation)
{
	rotationMatrix = glm::mat4_cast(glm::normalize(rotation));
}

void Transform::setScale(glm::vec3 scale)
{
	scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
}

void Transform::uniform(Shader& shader)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "translation"), 1, GL_FALSE, glm::value_ptr(translationMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "rotation"), 1, GL_FALSE, glm::value_ptr(rotationMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "scale"), 1, GL_FALSE, glm::value_ptr(scaleMatrix));
}

const glm::mat4 Transform::getTranslationMatrix()
{
	return translationMatrix;
}

const glm::mat4 Transform::getRotationMatrix()
{
	return rotationMatrix;
}

const glm::mat4 Transform::getScaleMatrix()
{
	return  scaleMatrix;
}