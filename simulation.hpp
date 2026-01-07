#pragma once

#include <memory>
#include <algorithm>

#include "planet.hpp"
#include "sun.hpp"
#include "satellite.hpp"

#include "circleIcon.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

const unsigned int OPENGL_VERSION_MAJOR = 4;
const unsigned int OPENGL_VERSION_MINOR = 6;
const unsigned int OPENGL_PROFILE = GLFW_OPENGL_CORE_PROFILE;

struct LaunchUI
{
	bool isOpen = false;
	char name[30];
	double dryMass= 1000.0;
	double fuelMass = 100.0;
	double latitudeDegrees = 0.0;
	double longitudeDegrees = 0.0;
	double azimuthDegrees = 90.0;
	double altitude_km = 200.0;
	double velocity = 7784.34;
	double flightPathAngleDegrees = 0.0;
	float colour[3] = { 1.0f, 1.0f, 1.0f };
};

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
	void simInfoUI();
	void fpsUI();
	void displayLaunchUI();

	void physicsUpdate();

	void addSatellite // adds a satellite to simulation
	(
		std::string name,
		double dryMass,
		double fuelMass,
		float colour[3],
		std::string planetName,
		double longitude, 
		double latitude, 
		double azimuth, 
		double altitude, 
		double velocity, 
		double flightPathAngle
	);
	void updateSatellites(); // does satellite updates
	void deleteSatellite(std::string name); // deletes a satellite via name
	void drawSatellites();

private:
	GLFWwindow* window;
	Camera camera;

	const char* windowTitle;

	int windowXPos;
	int windowYPos;
	int windowWidth;
	int windowHeight;
	float xScale;
	float yScale;

	bool initialised = false;
	bool paused = false;

	bool fullscreen = false;
	bool multisample = true;
	bool displayControls = true;
	bool displaySimInfo = true;
	bool displayFPS = false;

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
	
	std::vector<std::unique_ptr<Mesh>> meshes;

	std::unique_ptr<Text> textLoader;
	std::unique_ptr<Shader> textShader;
	std::unique_ptr<Shader> iconShader;
	std::unique_ptr<Shader> planetShader;
	std::unique_ptr<Shader> atmosphereShader;
	std::unique_ptr<Shader> sunShader;

	std::unique_ptr<Planet> earth;

	std::vector<std::unique_ptr<Planet>> planets;
	
	std::unique_ptr<Sun> sun;

	std::vector<Satellite> satellites;

	LaunchUI launchUI;
};