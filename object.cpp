#include "object.hpp"

Object::Object(unsigned int ID, std::string name)
{
	objectID = ID;
	objectName = name;
}

unsigned int Object::getID()
{
	return objectID;
}

std::string Object::getName()
{
	return objectName;
}
