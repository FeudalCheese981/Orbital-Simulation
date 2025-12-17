#pragma once

#include "shader.hpp"
#include "camera.hpp"

class Object
{
public:
	virtual ~Object() = default;

	virtual void draw(Shader& shader, Camera& camera) = 0;

	unsigned int getID();
	std::string getName();

protected:
	Object(unsigned int ID, std::string name);

	unsigned int objectID;
	std::string objectName;
};