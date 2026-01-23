#include "shader.hpp"

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	// load source code of shaders
	std::string vertexCode = readFileContents(vertexFile); 
	std::string fragmentCode = readFileContents(fragmentFile);
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// log shader errors at runtime
	int success; 
	char infoLog[512];

	// Vertex Shader:
	// create shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); 
	// give opengl the shader source code
	glShaderSource(vertexShader, 1, &vertexSource, NULL); 
	// compile the shader at runtime
	glCompileShader(vertexShader);
	// check status of compilation
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); 
	if (!success)
	{
		// get error info
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog); 
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment Shader
	// create shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// give opengl the shader source code
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// give opengl the shader source code
	glCompileShader(fragmentShader);
	// check status of compilation
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		// get error info
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Shader program:
	// creates shader program
	ID = glCreateProgram(); 
	// add shaders to shader program
	glAttachShader(ID, vertexShader); 
	glAttachShader(ID, fragmentShader);
	// link shaders in program
	glLinkProgram(ID); 
	// check status of linking
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		// get error info
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// delete individual shaders, they are now part of the program
	glDeleteShader(vertexShader); 
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