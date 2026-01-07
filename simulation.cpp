#include "simulation.hpp"

Simulation::Simulation(const char* title, int width, int height, int xPos, int yPos)
	: camera(width, height)
{
	windowTitle = title;
	windowWidth = width;
	windowHeight = height;
	windowXPos = xPos;
	windowYPos = yPos;

	if (!glfwInit())
	{
		std::cerr << "Failed to initialise GLFW!\n";
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, OPENGL_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);

	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window!\n";
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialise GLAD\n";
		glfwTerminate();
		return;
	}

	glfwSetWindowPos(window, windowXPos, windowYPos);
	glfwSetWindowUserPointer(window, (void*)this);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwGetWindowContentScale(window, &xScale, &yScale);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	ImGui::GetStyle();
	ImGui::StyleColorsDark();
	io->Fonts->AddFontFromFileTTF("fonts/DejaVuSans.ttf", 15.0f * xScale);

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	textLoader = std::make_unique<Text>(16);
	textShader = std::make_unique<Shader>("text.vert", "text.frag");
	iconShader = std::make_unique<Shader>("icon.vert", "icon.frag");

	glm::mat distanceScale = glm::mat4(1.0f);
	glm::vec3 scale = glm::vec3(1.0f / 6371000.0f);
	distanceScale = glm::scale(distanceScale, scale);
	camera.setDistanceScale(scale);

	planetShader = std::make_unique<Shader>("mesh.vert", "planet.frag");
	atmosphereShader = std::make_unique<Shader>("mesh.vert", "atmosphere.frag");
	glm::quat rotation = glm::normalize(glm::angleAxis(glm::radians(24.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f))));

	earth = std::make_unique<Planet>(
		"Earth",
		glm::vec3(0.0f, 0.0f, 0.0f),
		rotation,
		glm::vec3(1.0f),
		6371000.0,
		100000.0,
		5.97e24,
		"textures/8k_earth_daymap.jpg",
		"textures/8k_earth_specular_map.png",
		"textures/8k_earth_nightmap.jpg",
		glm::vec3(0.3f, 0.5f, 0.6f)
	);

	sunShader = std::make_unique<Shader>("mesh.vert", "sun.frag");
	sun = std::make_unique<Sun>(
		glm::vec3(0.0f, -150.0e9f, 0.0f),
		glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f),
		696000000.0,
		1.989e30,
		glm::vec3(1.0f, 1.0f, 1.0f)
	);

	sun->sendLightInfoToShader(*atmosphereShader);
	sun->sendLightInfoToShader(*planetShader);

	planetShader->activate();
	glUniformMatrix4fv(glGetUniformLocation(planetShader->getID(), "distanceScale"), 1, GL_FALSE, glm::value_ptr(distanceScale));

	atmosphereShader->activate();
	glUniformMatrix4fv(glGetUniformLocation(atmosphereShader->getID(), "distanceScale"), 1, GL_FALSE, glm::value_ptr(distanceScale));

	sunShader->activate();
	glUniformMatrix4fv(glGetUniformLocation(sunShader->getID(), "distanceScale"), 1, GL_FALSE, glm::value_ptr(distanceScale));

	// for Reversed-Z
	//glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	//glDepthFunc(GL_GREATER);

	glEnable(GL_DEPTH_TEST);
	

	glEnable(GL_MULTISAMPLE);

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	initialised = true;
}

Simulation::~Simulation()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Simulation::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Simulation* simPtr = (Simulation*)glfwGetWindowUserPointer(window);
	simPtr->resizeWindow(width, height);
}

void Simulation::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Simulation* simPtr = (Simulation*)glfwGetWindowUserPointer(window);
	simPtr->keyInput(key, scancode, action, mods);
}

void Simulation::scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	Simulation* simPtr = (Simulation*)glfwGetWindowUserPointer(window);
	simPtr->scrollInput(xOffset, yOffset);
}

void Simulation::resizeWindow(int width, int height)
{
	if (width && height)
	{
		glViewport(0, 0, width, height);
		camera.windowSizeUpdate(width, height);
	}
}

void Simulation::keyInput(int key, int scancode, int action, int mods)
{
	if (io->WantCaptureKeyboard)
		return;

	if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		if (!fullscreen)
		{
			glfwGetWindowPos(window, &windowXPos, &windowYPos);
			glfwGetWindowSize(window, &windowWidth, &windowHeight);
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
			fullscreen = true;
		}
		else
		{
			glfwSetWindowMonitor(window, NULL, windowXPos, windowYPos, windowWidth, windowHeight, GLFW_DONT_CARE);
			fullscreen = false;
		}
	}
	if (key == GLFW_KEY_V && action == GLFW_PRESS)
		camera.changeMode();
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
		camera.resetView();
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		simRate *= 2.0;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		simRate /= 2.0;
	if (key == GLFW_KEY_HOME && action == GLFW_PRESS)
		simRate = 1.0;
	if (key == GLFW_KEY_PAUSE && action == GLFW_PRESS)
	{
		if (paused)
		{
			paused = false;
			simRate = 1.0;
		}
		else
		{
			paused = true;
			simRate = 0.0;
		}
	}
}

void Simulation::scrollInput(double xOffset, double yOffset)
{
	if (io->WantCaptureMouse)
		return;
	camera.scrollInput(yOffset);
}

void Simulation::mainloop() 
{
	if (!initialised)
		return;

	double prevTime = glfwGetTime();
	double crntTime;
	fpsPrevDisplayTime = prevTime;

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//glClearDepth(0.0);
		glClear(GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glfwPollEvents();

		if (!io->WantCaptureKeyboard)
			camera.keyInput(window);
		if (!io->WantCaptureMouse)
			camera.mouseInput(window);

		camera.updateMatrix();
		displayUI();

		crntTime = glfwGetTime();
		fpsCrntDisplayTime = crntTime;

		frameTime = crntTime - prevTime;
		prevTime = crntTime;

		accumulator += frameTime;
		runTime += frameTime;

		updateFPS();

		while (accumulator >= deltaTime)
		{
			accumulator -= deltaTime;
			simTime += deltaTime * simRate;
			physicsUpdate();
		}
		
		draw();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(window);
	}
}

void Simulation::updateFPS()
{
	double fps = 1.0 / frameTime;
	fpsTrack.push_back(fps);
	if (fpsTrack.size() > 30) 
		fpsTrack.erase(fpsTrack.begin());
	if (fpsCrntDisplayTime - fpsPrevDisplayTime >= 1.0 / 30.0)
	{
		currentFPS = fps;
		double total = 0.0;
		for (int i = 0; i < fpsTrack.size(); i++)
		{
			total += fpsTrack[i];
		}
		averageFPS = total / 30;

		fpsPrevDisplayTime = fpsCrntDisplayTime;
	}
}

void Simulation::draw()
{
	sun->draw(*sunShader, camera);
	earth->draw(*planetShader, *atmosphereShader, camera);

	drawSatellites();
}

void Simulation::displayUI()
{
	mainMenuUI();
	controlsUI();
	simInfoUI();
	fpsUI();
	displayLaunchUI();
}

void Simulation::mainMenuUI() 
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Controls", "", &displayControls);
			ImGui::Separator();
			if (ImGui::MenuItem("Quit", "Alt+F4"))
			{
				glfwSetWindowShouldClose(window, true);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug"))
		{
			ImGui::MenuItem("Display Sim Info", "", &displaySimInfo);
			ImGui::MenuItem("Display FPS", "", &displayFPS);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Launch"))
		{
			ImGui::MenuItem("Launch Satellite", "", &launchUI.isOpen);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void Simulation::controlsUI()
{
	if (displayControls)
	{
		if (ImGui::Begin("Controls", &displayControls))
		{
			ImGui::SeparatorText("Camera Controls");
			ImGui::Text("Reset Camera:"); ImGui::SameLine(); ImGui::TextDisabled("C");
			ImGui::Text("Change Camera Mode:"); ImGui::SameLine(); ImGui::TextDisabled("v");
			ImGui::Text("Look around:"); ImGui::SameLine(); ImGui::TextDisabled("Hold Left Click");
			ImGui::SeparatorText("Movement Controls");
			ImGui::TextDisabled("(Only in free camera mode)");
			ImGui::Text("Forward:"); ImGui::SameLine(); ImGui::TextDisabled("W");
			ImGui::Text("Left:"); ImGui::SameLine(); ImGui::TextDisabled("A");
			ImGui::Text("Backward:"); ImGui::SameLine(); ImGui::TextDisabled("S");
			ImGui::Text("Right:"); ImGui::SameLine(); ImGui::TextDisabled("D");
			ImGui::Text("Up:"); ImGui::SameLine(); ImGui::TextDisabled("Space");
			ImGui::Text("Down:"); ImGui::SameLine(); ImGui::TextDisabled("Ctrl");
			ImGui::Text("Speed:"); ImGui::SameLine(); ImGui::TextDisabled("Shift");
			ImGui::SeparatorText("Sim Controls");
			ImGui::Text("Increase Sim Rate:"); ImGui::SameLine(); ImGui::TextDisabled("Up");
			ImGui::Text("Decrease Sim Rate:"); ImGui::SameLine(); ImGui::TextDisabled("Down");
			ImGui::Text("Reset Sim Rate:"); ImGui::SameLine(); ImGui::TextDisabled("Home");
		}
		ImGui::End();
	}
}

void Simulation::simInfoUI() 
{
	if (displaySimInfo)
	{
		if (ImGui::Begin("Sim Info", &displaySimInfo))
		{
			ImGui::Text("Sim Rate: %.2fX", simRate);
			ImGui::Text("Sim Time: %.2fs", simTime);
			ImGui::Separator();
			ImGui::Text("ΔT : %.3fs", deltaTime);
			ImGui::Text("Run Time: %.2fs", runTime);
		}
		ImGui::End();
	}
}

void Simulation::fpsUI()
{
	if (displayFPS)
	{
		if (ImGui::Begin("FPS stats", &displayFPS))
		{
			ImGui::Text("Current FPS: %.2f", currentFPS);
			ImGui::Text("Average FPS: %.2f", averageFPS);
		}
		ImGui::End();
	}
}

void Simulation::displayLaunchUI()
{
	if (launchUI.isOpen)
	{
		if (ImGui::Begin("Launch##", &launchUI.isOpen))
		{
			ImGui::SeparatorText("Satellite Info");
			ImGui::Text("Name");
			ImGui::InputTextWithHint("##Name", "Name", launchUI.name, 30);
			ImGui::Text("Dry Mass");
			ImGui::InputDouble("kg##DryMass", &launchUI.dryMass);
			ImGui::Text("Fuel Mass");
			ImGui::InputDouble("kg##FuelMas", &launchUI.fuelMass);

			ImGui::SeparatorText("Launch Info");
			ImGui::Text("Planet");
			
			ImGui::Text("Longitude");
			ImGui::InputDouble("°##Longitude", &launchUI.longitudeDegrees);
			ImGui::Text("Latitude");
			ImGui::InputDouble("°##Latitude", &launchUI.latitudeDegrees);
			ImGui::Text("Azimuth");
			ImGui::InputDouble("°##Azimuth", &launchUI.azimuthDegrees);

			ImGui::SeparatorText("Engine Cutoff");
			ImGui::Text("Altitude");
			ImGui::InputDouble("km##Altiude", &launchUI.altitude_km);
			ImGui::Text("Velocity");
			ImGui::InputDouble("m/s##Velocity", &launchUI.velocity);
			ImGui::Text("Flight Path Angle");
			ImGui::InputDouble("°##FlightPathAngle", &launchUI.flightPathAngleDegrees);
			
			ImGui::Separator();
			ImGui::ColorEdit3("##Colour", launchUI.colour);
			if (ImGui::Button("Launch!"))
			{
				launchUI.isOpen = false;
				addSatellite
				(
					launchUI.name,
					launchUI.dryMass,
					launchUI.fuelMass,
					launchUI.colour,
					"Earth",
					glm::radians(launchUI.longitudeDegrees),
					glm::radians(launchUI.latitudeDegrees),
					glm::radians(launchUI.azimuthDegrees),
					launchUI.altitude_km * 1000,
					launchUI.velocity,
					glm::radians(launchUI.flightPathAngleDegrees)
				);
			}
		}
		ImGui::End();
	}
	else
	{
		launchUI = LaunchUI{};
	}
}

void Simulation::physicsUpdate()
{
	earth->rotateAboutAxis(deltaTime * simRate, 86400);
	updateSatellites();
}

void Simulation::addSatellite
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
)
{
	Planet* planetPtr = earth.get();
	for (int i = 0; i < planets.size(); i++)
	{
		if (planets[i]->getName() == planetName)
		{
			planetPtr = planets[i].get();
			break;
		}
	}
	satellites.emplace_back
	(
		name,
		dryMass,
		fuelMass,
		glm::vec4(colour[0], colour[1], colour[2], 1.0f),
		planetPtr,
		longitude,
		latitude,
		azimuth,
		altitude,
		velocity,
		flightPathAngle,
		simTime
	);
}

void Simulation::updateSatellites()
{
	for (int i = 0; i < satellites.size(); i++)
	{
		Satellite& satellite = satellites[i];
		satellite.updatePosition(simTime);
	}
}

void Simulation::deleteSatellite(std::string name)
{
	satellites.erase
	(
		std::remove_if
		(
			satellites.begin(),
			satellites.end(),
			[name](Satellite& satellite) {return satellite.getName() == name;}
		),
		satellites.end()
	);
}

void Simulation::drawSatellites()
{
	for (int i = 0; i < satellites.size(); i++)
	{
		Satellite& satellite = satellites[i];
		satellite.draw(*iconShader, *textShader, *sunShader, camera, *textLoader, xScale);
	}
}