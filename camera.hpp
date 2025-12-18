#pragma once

#include "shader.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

enum CameraMode { FREE, ORBITAL };

class Camera
{
public:
	Camera(int width, int height);
	~Camera() = default;
	
	void windowSizeUpdate(int width, int height);
	void updateMatrix();
	void cameraUniform(Shader& shader);

	void changeFOV(float newFOVdeg);
	void changeSensitivity(float newSensitivity);
	void changeMode();
	void resetView();

	glm::vec3 getPos();
	glm::mat4 getMatrix();
	glm::mat4 getOrthogonalProjection();
	glm::vec4 orthogonalDisplay(glm::vec3 pos);

	void keyInput(GLFWwindow* window);
	void mouseInput(GLFWwindow* window);
	void scrollInput(double yOffset);

private:
	glm::vec3 position; // stores the current camera position
	glm::vec3 orientation; // stores the current camera orientation
	glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f); // up is defined as positive z
	glm::mat4 cameraMatrix = glm::mat4(1.0f); // initalize camera matrix to identiy matrix
	glm::mat4 orthogonalProjection = glm::mat4(1.0f);

	glm::vec3 positionStore; // stores the position when switching modes
	glm::vec3 orientationStore; // stores the orientation when switching modes

	bool firstClick = true; // stores whether a registered click is the first in the stream
	double mouseClickX = 0.0; // store the position of a mouse click
	double mouseClickY = 0.0;

	int windowWidth; // store of the window width and height
	int windowHeight;

	int mode = FREE; // set the initial camera mode to free camera

	float speed = 0.01f; // default speed
	float sensitivity = 150.0f; // default sensitivity
	float FOV = glm::radians(45.0f); // default Field of View in radians
	float nearPlane = 0.01f; // near plane distance of view frustum
	float farPlane = 1.0e6f; // far plane distance of view frustum
};