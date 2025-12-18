#pragma once

#include <memory>
#include <algorithm>

#include "texturedMesh.hpp"
#include "untexturedMesh.hpp"

#include "circleIcon.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

const unsigned int OPENGL_VERSION_MAJOR = 4;
const unsigned int OPENGL_VERSION_MINOR = 6;
const unsigned int OPENGL_PROFILE = GLFW_OPENGL_CORE_PROFILE;

class Simulation
{
public:
	Simulation(const char* title, int width, int height, int xPos, int yPos);
	~Simulation();

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

	void mainloop();
	
	void updateFPS();
	void draw();

	void resizeWindow(int width, int height);
	void keyInput(int key, int scancode, int action, int mods);
	void scrollInput(double xOffset, double yOffset);

	void displayUI();
	void mainMenuUI();
	void controlsUI();
	void SimInfoUI();
	void fpsUI();

private:
	GLFWwindow* window;
	Camera camera;

	const char* windowTitle;

	int windowXPos;
	int windowYPos;
	int windowWidth;
	int windowHeight;

	bool initialised = false;

	bool fullscreen = false;
	bool multisample = true;

	double accumulator = 0.0;
	double deltaTime = 1.0 / 1000.0;

	double runTime = 0.0;
	double simTime = 0.0;
	double simRate = 1.0;

	double fpsPrevDisplayTime = 0.0;
	double fpsCrntDisplayTime = 0.0;
	double frameTime = 0.0;
	double currentFPS = 0.0;
	double averageFPS = 0.0;

	std::vector<double> fpsTrack;

	ImGuiIO* io = nullptr;
	
	std::vector<std::unique_ptr<UntexturedMesh>> meshes;

	std::unique_ptr<Text> textLoader;

	std::unique_ptr<Shader> shaderProgram;
	std::unique_ptr<UntexturedMesh> triangle;

	std::unique_ptr<Shader> textShader;
	std::unique_ptr<Shader> iconShader;

	std::unique_ptr<CircleIcon> icon1;
	std::unique_ptr<CircleIcon> icon2;
	std::unique_ptr<CircleIcon> icon3;
};