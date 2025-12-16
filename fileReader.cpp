#include "fileReader.hpp"

std::string readFileContents(const char* filename)
{
	std::ifstream file(filename, std::ios::binary); // attempt to load file
	if (!file)
		throw std::runtime_error("Could not open file: "); // throw exception if file not found

	std::string contents((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()) ); // store contents in a string
	return contents; // return the file contents
}