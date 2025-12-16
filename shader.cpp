#include "shader.hpp"

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	std::string vertexCode = readFileContents(vertexFile); // load source code of shaders
	std::string fragmentCode = readFileContents(fragmentFile);
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// as shader compilation happens at runtime, we can log errors instead of throwing them
	int success; // variable to keep track of whether compilation/ linking is successful
	char infoLog[512];

	// Vertex Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); // create shader
	glShaderSource(vertexShader, 1, &vertexSource, NULL); // give opengl the shader source code
	glCompileShader(vertexShader);// compile the shader at runtime

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // check status of compilation
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog); // get error info
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment Shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// creating Shader program
	ID = glCreateProgram(); // creates shader program
	glAttachShader(ID, vertexShader); // add shaders to shader program
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID); // Link shaders in program

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader); // Delete individual shaders now that they are part of the shader program
	glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

void Shader::activate()
{
	glUseProgram(ID);
}

GLuint Shader::getID()
{
	return ID;
}