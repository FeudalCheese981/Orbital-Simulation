#pragma once

#include <memory>
#include <algorithm>

#include "planet.hpp"
#include "sun.hpp"
#include "satellite.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

const unsigned int OPENGL_VERSION_MAJOR = 4; // defining the OpenGL version/profile as a constant for clarity
const unsigned int OPENGL_VERSION_MINOR = 6;
const unsigned int OPENGL_PROFILE = GLFW_OPENGL_CORE_PROFILE;

const unsigned int DEFAULT_FONT_SIZE = 15;

// struct containing data for inputs within the user interface launch window
struct LaunchUI
{
	bool isOpen = false;
	char name[30] = "Unnamed Satellite";
	double dryMass= 1000.0;
	double fuelMass = 100.0;
	double latitudeDegrees = 0.0;
	double longitudeDegrees = 0.0;
	double azimuthDegrees = 90.0;
	double altitude_km = 200.0;
	double velocity = 7784.34;
	double flightPathAngleDegrees = 0.0;
	float colour[3] = { 1.0f, 1.0f, 1.0f };
	bool nameTaken = false;
};

// Simulation class, 
class Simulation
{
public:
	Simulation(const char* title, int width, int height, int xPos, int yPos);
	~Simulation();

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

	void mainloop(); // the main rendering loop
	
	void updateFPS(); // updates the current FPS
	void draw(); // draws all objects in the simulation

	void resizeWindow(int width, int height); // handles window resizing
	void keyInput(int key, int scancode, int action, int mods); // handles key input 
	void scrollInput(double xOffset, double yOffset); // handles the user using the scroll wheel

	void displayUI(); // main display call for the User Interface
	void mainMenuUI(); // main menu
	void controlsUI(); // displays controls to the user
	void simInfoUI(); // displays information about the simulation
	void fpsUI(); // displays the FPS
	void launchUI(); // UI for user launching a satellite
	void satelliteUI(); // displays information about the satellite
	void destroyPromptUI(); // prompt for user to conmfirm destroying satellite

	void physicsUpdate(); // updates physics of all objects within simulation

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
	void updateSatellites(); // helper function that does satellite physics updates
	void deleteSatellite(std::string name); // deletes a satellite via name
	void drawSatellites(); // helper function called by draw() to draw satellites specifically

private:
	GLFWwindow* window; // stores pointer to the window
	Camera camera; // instance of the camera

	const char* windowTitle; // stores the title of the window

	int windowXPos; // window position and size information
	int windowYPos;
	int windowWidth;
	int windowHeight;

	float xScale; // stores the DPI scale for scaling the user interface
	float yScale;

	bool initialised = false; // boolean variable tracks whether the simuilation has succesfully initialised
	bool paused = false; // tracks whether the simulation is paused

	bool fullscreen = false; // user is/isn't on fullscreen
	bool multisample = true; // multisampling enabled/disabled

	bool displayControls = true; // UI elements are displayed/hidden
	bool displaySimInfo = true;
	bool displayFPS = false;
	bool destroyPrompt = false;

	std::string destroyName; // stores name of satellite to destroy

	double accumulator = 0.0; // for physics updates every frame
	double deltaTime = 1.0 / 1000.0;

	double runTime = 0.0; // other simulation information
	double simTime = 0.0;
	double simRate = 1.0;

	double fpsPrevDisplayTime = 0.0; // fps data
	double fpsCrntDisplayTime = 0.0;
	double frameTime = 0.0;
	double currentFPS = 0.0;
	double averageFPS = 0.0;

	std::vector<double> fpsTrack; // tracking fps for an average

	ImGuiIO* io = nullptr; // pointer for User Interface

	std::unique_ptr<Text> textLoader; // for text rendering
	std::unique_ptr<Shader> textShader;
	std::unique_ptr<Shader> iconShader;

	std::unique_ptr<Shader> planetShader; // Shaders for planets/sun
	std::unique_ptr<Shader> atmosphereShader;
	std::unique_ptr<Shader> sunShader;

	std::unique_ptr<Planet> earth;
	
	std::unique_ptr<Sun> sun;

	std::vector<Satellite> satellites;

	LaunchUI launchUIdata; // storing struct as an attribute for fetching data between frames
};