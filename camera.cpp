#include "camera.hpp"

Camera::Camera(int width, int height)
{
	// set the Window size and the position
	windowSizeUpdate(width, height);
	resetView();
}

void Camera::windowSizeUpdate(int width, int height)
{
	// Set the new Height And Width
	windowWidth = width;
	windowHeight = height;
	
	// Update the Orthogonal Projection Matrix
	orthogonalProjection = glm::ortho(0.0f, (float)(width), 0.0f, (float)(height));
}

void Camera::updateMatrix()
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Set view and projection matrix
	view = glm::lookAt(position, position + orientation, up);
	projection = glm::perspective(FOV, (float)(windowWidth) / (float)(windowHeight), nearPlane, farPlane);

	// Set the camera Matrix
	cameraMatrix = projection * view; 
}

void Camera::cameraUniform(Shader& shader)
{
	// Send Camera Matrix and Position to the Shader
	glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
	glUniform3f(glGetUniformLocation(shader.getID(), "cameraPosition"), position.x, position.y, position.z);
}

void Camera::changeFOV(float newFOVdeg)
{
	FOV = glm::radians(newFOVdeg);
}

void Camera::changeSensitivity(float newSensitivity)
{
	sensitivity = newSensitivity;
}

void Camera::changeMode()
{
	// Temporarily store the position and Orientation
	glm::vec3 tempPositon = position;
	glm::vec3 tempOrientation = orientation;
	
	// Set the new Camera Mode
	if (mode == ORBITAL)
	{
		mode = FREE;
	}
	else if (mode == FREE)
	{
		mode = ORBITAL;
	}

	// Update the position and orientation
	position = positionStore;
	orientation = orientationStore;
	// Set the storage variables
	positionStore = tempPositon;
	orientationStore = tempOrientation;
}

void Camera::resetView()
{
	// Set default camera position and orientation
	position = glm::vec3(1.0f, 1.0f, 1.0f);
	orientation = glm::normalize(-position);
	// Set the storage attributes to these initial values
	positionStore = position;
	orientationStore = orientation;
}

void Camera::setDistanceScale(glm::vec3 scale)
{
	distanceScale = scale;
}

glm::vec3 Camera::getPos()
{
	return position;
}

glm::mat4 Camera::getMatrix()
{
	return cameraMatrix;
}

glm::mat4 Camera::getOrthogonalProjection()
{
	return orthogonalProjection;
}

glm::vec4 Camera::orthogonalDisplay(glm::vec3 pos)
{
	// Convert 3D Coordinates into relative screen coordinates (-1 to 1)
	glm::vec4 screenPos = cameraMatrix * glm::vec4(pos * distanceScale, 1.0f);
	// normalise coordinates with distance
	glm::vec2 normalisedCoords = glm::vec2(screenPos) / screenPos.w;
	// compute x and y window coordinates
	float x = (normalisedCoords.x * 0.5f + 0.5f) * windowWidth;
	float y = (normalisedCoords.y * 0.5f + 0.5f) * windowHeight;

	return glm::vec4(x, y, 0.0f, screenPos.w);
}

glm::vec3 Camera::getDistanceScale()
{
	return distanceScale;
}

void Camera::keyInput(GLFWwindow* window)
{
	if (mode == FREE)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			position += speed * glm::normalize(orientation);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			position += speed * -glm::normalize(orientation);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			position += speed * glm::normalize(glm::cross(orientation, up));
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			position += speed * -glm::normalize(glm::cross(orientation, up));
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			position += speed * glm::normalize(up);
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			position += speed * -glm::normalize(up);
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			speed = 0.1f;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		{
			speed = 0.01f;
		}
	}
}

void Camera::mouseInput(GLFWwindow* window)
{
	if (mode == FREE)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			// Hide the mouse cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			
			// If the click is the first click in the data stream set the mouse to the coordinates of the click location
			if (firstClick)
			{
				glfwSetCursorPos(window, mouseClickX, mouseClickY);
				firstClick = false;
			}

			// Get new cursor position
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			// Calculate rotation in x and y
			float rotX = - sensitivity * (float)(mouseX - mouseClickX) / (float)(windowWidth);
			float rotY = - sensitivity * (float)(mouseY - mouseClickY) / (float)(windowHeight);

			// Rotate orientation in y with max angle 85 degrees
			glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(rotY), glm::normalize(glm::cross(orientation, up)));
			if (abs(glm::angle(glm::normalize(newOrientation), up) - glm::radians(90.0f)) <= glm::radians(85.0f))
			{
				orientation = newOrientation;
			}
			// Rotate orientation in x
			orientation = glm::rotate(orientation, glm::radians(rotX), up);

			// Reset cursor position to click spot
			glfwSetCursorPos(window, mouseClickX, mouseClickY);
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
		{
			// Show mouse cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstClick = true;
			// Get the mouse position for next click
			glfwGetCursorPos(window, &mouseClickX, &mouseClickY);
		}
	}

	// Same process for FREE camera, except for poition also moves with orientation
	else if (mode == ORBITAL)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			if (firstClick)
			{
				glfwSetCursorPos(window, mouseClickX, mouseClickY);
				firstClick = false;
			}

			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			float rotX = -sensitivity * (float)(mouseX - mouseClickX) / (float)(windowWidth);
			float rotY = -sensitivity * (float)(mouseY - mouseClickY) / (float)(windowHeight);

			glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(rotY), glm::normalize(glm::cross(orientation, up)));
			glm::vec3 newPosition = glm::rotate(position, glm::radians(rotY), glm::normalize(glm::cross(orientation, up)));

			if (abs(glm::angle(glm::normalize(newOrientation), up) - glm::radians(90.0f)) <= glm::radians(85.0f))
			{
				orientation = newOrientation;
				position = newPosition;
			}

			orientation = glm::rotate(orientation, glm::radians(rotX), up);
			position = glm::rotate(position, glm::radians(rotX), up);

			glfwSetCursorPos(window, mouseClickX, mouseClickY);
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstClick = true;
			glfwGetCursorPos(window, &mouseClickX, &mouseClickY);
		}
	}
}

void Camera::scrollInput(double yOffset)
{
	if (mode == ORBITAL)
	{
		glm::vec3 newPosition = position + (float)yOffset * orientation * glm::length(position) / 10.0f;

		if (glm::length(newPosition) >= 1.25f)
		{
			position = newPosition; // Camera can only approach a certain distance
		}

		else
		{
			position = glm::normalize(position) * 1.25f; // So camera doesnt get stuck
		}
	}
}