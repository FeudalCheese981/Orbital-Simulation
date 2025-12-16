#include "camera.hpp"

Camera::Camera(int width, int height)
{
	windowSizeUpdate(width, height);
	resetView();
}

void Camera::windowSizeUpdate(int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	
	orthogonalProjection = glm::ortho(0.0f, (float)(width), 0.0f, (float)(height));
}

void Camera::updateMatrix()
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	view = glm::lookAt(position, position + orientation, up);
	projection = glm::perspective(FOV, (float)(windowWidth) / (float)(windowHeight), nearPlane, farPlane);

	cameraMatrix = projection * view;
}

void Camera::cameraUniform(Shader& shader)
{
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
	glm::vec3 tempPositon = position;
	glm::vec3 tempOrientation = orientation;
	

	if (mode == ORBITAL)
	{
		mode = FREE;
	}
	else if (mode == FREE)
	{
		mode = ORBITAL;
	}

	position = positionStore;
	orientation = orientationStore;
	positionStore = tempPositon;
	orientationStore = tempOrientation;
}

void Camera::resetView()
{
	position = glm::vec3(1.0f, 1.0f, 1.0f);
	orientation = glm::normalize(-position);
	positionStore = position;
	orientationStore = orientation;
}

glm::mat4 Camera::getMatrix()
{
	return cameraMatrix;
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
			speed = 0.01f;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		{
			speed = 0.1f;
		}
	}
}

void Camera::mouseInput(GLFWwindow* window)
{
	if (mode == FREE)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		
			if (firstClick)
			{
				glfwSetInputMode(window, mouseClickX, mouseClickY);
				firstClick = false;
			}

			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			float rotX = - sensitivity * (float)(mouseX - mouseClickX) / (float)(windowWidth);
			float rotY = - sensitivity * (float)(mouseY - mouseClickY) / (float)(windowHeight);

			glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(rotY), glm::normalize(glm::cross(orientation, up)));

			if (abs(glm::angle(glm::normalize(newOrientation), up) - glm::radians(90.0f)) <= glm::radians(90.0f))
			{
				orientation = newOrientation;
			}

			orientation = glm::rotate(orientation, glm::radians(rotX), up);

			glfwSetCursorPos(window, mouseClickX, mouseClickY);
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstClick = true;
			glfwGetCursorPos(window, &mouseClickX, &mouseClickY);
		}
	}

	else if (mode == ORBITAL)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			if (firstClick)
			{
				glfwSetInputMode(window, mouseClickX, mouseClickY);
				firstClick = false;
			}

			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			float rotX = -sensitivity * (float)(mouseX - mouseClickX) / (float)(windowWidth);
			float rotY = -sensitivity * (float)(mouseY - mouseClickY) / (float)(windowHeight);

			glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(rotY), glm::normalize(glm::cross(orientation, up)));
			glm::vec3 newPosition = glm::rotate(position, glm::radians(rotY), glm::normalize(glm::cross(orientation, up)));

			if (abs(glm::angle(glm::normalize(newOrientation), up) - glm::radians(90.0f)) <= glm::radians(90.0f))
			{
				orientation = newOrientation;
				position = newPosition;
			}

			orientation = glm::rotate(orientation, glm::radians(rotX), up);
			position = glm::rotate(orientation, glm::radians(rotX), up);

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