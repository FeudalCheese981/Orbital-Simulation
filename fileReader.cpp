#include "fileReader.hpp"

std::string readFileContents(const char* filename)
{
	// Attempt to load file
	std::ifstream file(filename, std::ios::binary);
	// Throw exception if file not found
	if (!file)
		throw std::runtime_error("Could not open file: "); 

	// Store contents in a string
	std::string contents((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()) );
	return contents; // Return the file contents
}