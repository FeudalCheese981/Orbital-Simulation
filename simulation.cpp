#include "simulation.hpp"

Simulation::Simulation(const char* title, int width, int height, int xPos, int yPos)
	: camera(width, height) // initialise camera
{
	windowTitle = title; // set window parameters
	windowWidth = width;
	windowHeight = height;
	windowXPos = xPos;
	windowYPos = yPos;

	if (!glfwInit()) // initialise GLFW with error checking
	{
		std::cerr << "Failed to initialise GLFW!\n";
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR); // tell GLFW the version of OpenGL being used
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, OPENGL_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 8); // enable multisampling

	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL); // attempt to create the window
	if (!window) // error checking if window fails to be created
	{
		std::cerr << "Failed to create GLFW window!\n";
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window); // makes the window the current context on the cpu

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // load glad procedures for OpenGL, with error checking
	{
		std::cerr << "Failed to initialise GLAD\n";
		glfwTerminate();
		return;
	}

	glfwSetWindowPos(window, windowXPos, windowYPos); // set the window position
	glfwSetWindowUserPointer(window, (void*)this); // sets pointer to itself (useful with framebuffer callback)
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // callbacks for user input
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwGetWindowContentScale(window, &xScale, &yScale); // gets the windows DPI scale

	IMGUI_CHECKVERSION(); // initialise ImGui user interface
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	ImGui::GetStyle();
	ImGui::StyleColorsDark();
	io->Fonts->AddFontFromFileTTF("fonts/DejaVuSans.ttf", DEFAULT_FONT_SIZE * xScale); // set font

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	textLoader = std::make_unique<Text>(DEFAULT_FONT_SIZE); // initalise text loader along with shaders
	textShader = std::make_unique<Shader>("text.vert", "text.frag");
	iconShader = std::make_unique<Shader>("icon.vert", "icon.frag");

	// setting distance scale so that earth has a relative size of 1
	glm::mat distanceScale = glm::mat4(1.0f);
	glm::vec3 scale = glm::vec3(1.0f / 6371000.0f);
	distanceScale = glm::scale(distanceScale, scale);
	camera.setDistanceScale(scale); 

	// load shaders for planet
	planetShader = std::make_unique<Shader>("mesh.vert", "planet.frag");
	atmosphereShader = std::make_unique<Shader>("mesh.vert", "atmosphere.frag");
	glm::quat rotation = glm::normalize(glm::angleAxis(glm::radians(24.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f))));

	// initialise earth
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

	// initialise sun and shaders
	sunShader = std::make_unique<Shader>("mesh.vert", "sun.frag");
	sun = std::make_unique<Sun>(
		glm::vec3(0.0f, -150.0e9f, 0.0f),
		glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f),
		696000000.0,
		1.989e30,
		glm::vec3(1.0f, 1.0f, 1.0f)
	);

	// pass sun info to planet shaders
	sun->sendLightInfoToShader(*atmosphereShader);
	sun->sendLightInfoToShader(*planetShader);

	// set distance scale in shaders
	planetShader->activate();
	glUniformMatrix4fv(glGetUniformLocation(planetShader->getID(), "distanceScale"), 1, GL_FALSE, glm::value_ptr(distanceScale));

	atmosphereShader->activate();
	glUniformMatrix4fv(glGetUniformLocation(atmosphereShader->getID(), "distanceScale"), 1, GL_FALSE, glm::value_ptr(distanceScale));

	sunShader->activate();
	glUniformMatrix4fv(glGetUniformLocation(sunShader->getID(), "distanceScale"), 1, GL_FALSE, glm::value_ptr(distanceScale));

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_MULTISAMPLE);

	// enable face culling
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// blending for transparent objects
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// simulation is initialised
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
		glViewport(0, 0, width, height); // adjust the viewport
		camera.windowSizeUpdate(width, height);

		// only update the width and height variables on window resize if windowed
		if (glfwGetWindowMonitor == nullptr)
		{
			windowWidth = width;
			windowHeight = height;
		}
	}
}

void Simulation::keyInput(int key, int scancode, int action, int mods)
{
	if (io->WantCaptureKeyboard)
		return;

	if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor); // get the window monitor
		if (!fullscreen)
		{
			glfwGetWindowPos(window, &windowXPos, &windowYPos); // store the window position
			glfwGetWindowSize(window, &windowWidth, &windowHeight); // store window height
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate); // set fullscreen mode
			fullscreen = true;
		}
		else
		{
			glfwSetWindowMonitor(window, NULL, windowXPos, windowYPos, windowWidth, windowHeight, GLFW_DONT_CARE); // go back to windowed mode
			fullscreen = false;
		}
	}
	if (key == GLFW_KEY_V && action == GLFW_PRESS)
		camera.changeMode();
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
		camera.resetView();
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) // double the sim rate
		simRate *= 2.0;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) // halve the sim rate
		simRate /= 2.0;
	if (key == GLFW_KEY_HOME && action == GLFW_PRESS) // reset the sim rate back to 1.0
		simRate = 1.0;
	if (key == GLFW_KEY_PAUSE && action == GLFW_PRESS)
	{
		if (paused) // unpause
		{
			paused = false;
			simRate = 1.0;
		}
		else // pauses by setting simRate to 0
		{
			paused = true;
			simRate = 0.0;
		}
	}
}

void Simulation::scrollInput(double xOffset, double yOffset)
{
	if (io->WantCaptureMouse) // chcek if UI wants to use the mouse
		return;
	camera.scrollInput(yOffset);
}

void Simulation::mainloop() 
{
	if (!initialised) // cant run if simulation not initialised
		return;

	double prevTime = glfwGetTime(); // get the current time for FPS and physics
	double crntTime;
	fpsPrevDisplayTime = prevTime;

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // clear buffers
		glClear(GL_COLOR_BUFFER_BIT);

		glClear(GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame(); // tell ImGui there is a new frame
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glfwPollEvents(); // poll inputs

		if (!io->WantCaptureKeyboard) // update sim based on inputs if not inputting into UI
			camera.keyInput(window);
		if (!io->WantCaptureMouse)
			camera.mouseInput(window);

		camera.updateMatrix(); // update the camera view and projection matrix
		displayUI(); // display UI to user every frame

		crntTime = glfwGetTime(); // update time-keeping variables
		fpsCrntDisplayTime = crntTime;

		frameTime = crntTime - prevTime;
		prevTime = crntTime;

		accumulator += frameTime; // increment accumulator and run time
		runTime += frameTime;

		updateFPS(); // update FPS every frame

		// physics loop
		// multiple physics updates can happen every frame
		while (accumulator >= deltaTime)
		{
			accumulator -= deltaTime;
			simTime += deltaTime * simRate;
			physicsUpdate();
		}
		
		draw(); // draw objects in scene
		ImGui::Render(); // tell ImGui to render UI
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(window); // swap buffers
	}
}

void Simulation::updateFPS()
{
	double fps = 1.0 / frameTime; // calculate current FPS
	fpsTrack.push_back(fps); // add to fps tracking vector
	if (fpsTrack.size() > 30) 
		fpsTrack.erase(fpsTrack.begin()); // limit fps tracking to last 30 frames
	if (fpsCrntDisplayTime - fpsPrevDisplayTime >= 1.0 / 30.0) // display/ update FPS every 1/30 a second
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
	// draw sun and earth and satellites
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
	launchUI();
	satelliteUI();
	destroyPromptUI();
}

void Simulation::mainMenuUI() 
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Controls", "", &displayControls); // user selects to display controls
			ImGui::Separator();
			if (ImGui::MenuItem("Quit", "Alt+F4")) // allows user to quit program
			{
				glfwSetWindowShouldClose(window, true);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug"))
		{
			ImGui::MenuItem("Display Sim Info", "", &displaySimInfo); // allows user to view information about sim
			ImGui::MenuItem("Display FPS", "", &displayFPS); // allows user to view fps
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Satellites"))
		{
			ImGui::MenuItem("Launch Satellite", "", &launchUIdata.isOpen); // allows user to launch a satellite

			if (satellites.size() != 0) // if there are satellites in the simulation adds a view menu for user to selct satellite
			{
				if (ImGui::BeginMenu("View"))
				{
					for (int i = 0; i < satellites.size(); i++)
					{
						Satellite& satellite = satellites[i];
						if (ImGui::MenuItem((satellite.getName() + "##").c_str(), "", satellite.selected))
						{
							// controlling satellite selection
							if (satellite.selected)
								satellite.selected = false;
							else
								satellite.selected = true;
						}
					}
					ImGui::EndMenu();
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void Simulation::controlsUI()
{
	if (displayControls)
	{
		// displays simulation controls to the user
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
		// in a window shows the simulation information
		if (ImGui::Begin("Sim Info", &displaySimInfo))
		{
			ImGui::Text("Sim Rate: %.2fX", simRate);
			ImGui::Text("Sim Time: %.2fs", simTime);
			ImGui::Separator();
			ImGui::Text("ΔT : %.3fs", deltaTime);
			ImGui::Text("Run Time: %.2fs", runTime);
			ImGui::Separator();
			ImGui::Text("No. of Satellites: %d", satellites.size());
		}
		ImGui::End();
	}
}

void Simulation::fpsUI()
{
	if (displayFPS)
	{
		// in a winow shows the FPS statistics
		if (ImGui::Begin("FPS stats", &displayFPS))
		{
			ImGui::Text("Current FPS: %.2f", currentFPS);
			ImGui::Text("Average FPS: %.2f", averageFPS);
		}
		ImGui::End();
	}
}

void Simulation::launchUI()
{
	if (launchUIdata.isOpen)
	{
		// allows user to enter launch parameters and launch satellites
		if (ImGui::Begin("Launch##", &launchUIdata.isOpen))
		{
			ImGui::SeparatorText("Satellite Info");
			ImGui::Text("Name");
			ImGui::InputTextWithHint("##Name", "Name", launchUIdata.name, 30);
			ImGui::Text("Dry Mass");
			ImGui::InputDouble("kg##DryMass", &launchUIdata.dryMass);
			ImGui::Text("Fuel Mass");
			ImGui::InputDouble("kg##FuelMas", &launchUIdata.fuelMass);

			ImGui::SeparatorText("Launch Info");
			ImGui::Text("Planet: Earth");
			
			ImGui::Text("Longitude");
			ImGui::InputDouble("°##Longitude", &launchUIdata.longitudeDegrees);
			ImGui::Text("Latitude");
			ImGui::InputDouble("°##Latitude", &launchUIdata.latitudeDegrees);
			ImGui::Text("Azimuth");
			ImGui::InputDouble("°##Azimuth", &launchUIdata.azimuthDegrees);

			ImGui::SeparatorText("Engine Cutoff");
			ImGui::Text("Altitude");
			ImGui::InputDouble("km##Altiude", &launchUIdata.altitude_km);
			ImGui::Text("Velocity");
			ImGui::InputDouble("m/s##Velocity", &launchUIdata.velocity);
			ImGui::Text("Flight Path Angle");
			ImGui::InputDouble("°##FlightPathAngle", &launchUIdata.flightPathAngleDegrees);
			
			ImGui::Separator();
			ImGui::ColorEdit3("##Colour", launchUIdata.colour);
			if (ImGui::Button("Launch!"))
			{
				// error checking so that user cant enter a satellite that has the same name as another
				launchUIdata.nameTaken = false;
				for (int i = 0; i < satellites.size(); i++)
				{
					Satellite& satellite = satellites[i];
					if (satellite.getName() == launchUIdata.name)
						launchUIdata.nameTaken = true;
				}

				if (!launchUIdata.nameTaken)
				{
					// if name not taken create satellite and close window
					launchUIdata.isOpen = false;
					addSatellite
					(
						launchUIdata.name,
						launchUIdata.dryMass,
						launchUIdata.fuelMass,
						launchUIdata.colour,
						"Earth",
						glm::radians(launchUIdata.longitudeDegrees),
						glm::radians(launchUIdata.latitudeDegrees),
						glm::radians(launchUIdata.azimuthDegrees),
						launchUIdata.altitude_km * 1000,
						launchUIdata.velocity,
						glm::radians(launchUIdata.flightPathAngleDegrees)
					);
				}
			}

			// outputs an error message if the user does enter a name for their satellite
			// that another satellite has
			if (launchUIdata.nameTaken)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				ImGui::Text("Cant Have two Satellites of the same name");
				ImGui::PopStyleColor();
			}
		}
		ImGui::End();
	}
	else
	{
		launchUIdata = LaunchUI{};
	}
}

void Simulation::satelliteUI()
{
	for (int i = 0; i < satellites.size(); i++)
	{
		// shows satellite info in a window for all satellites that are selected
		Satellite& satellite = satellites[i];
		if (satellite.selected)
		{
			std::string name = satellite.getName();

			ImGui::SetNextWindowPos(ImVec2(windowWidth - 250 * xScale - 50, 50), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(250 * xScale, 280 * yScale), ImGuiCond_Once);
			if (ImGui::Begin((satellite.getName() + "##").c_str(), &satellite.selected))
			{
				// get and display satellite parameters
				ImGui::Text("Altitude: %.2fkm", satellite.getAltitude() / 1000.0);
				ImGui::Text("Velocity: %.2fm/s", satellite.getVelocity());
				ImGui::Text("FPA: %.2f°", glm::degrees(satellite.getFlightPathAngle()));
				ImGui::Separator();
				ImGui::Text("Apoapsis: %.2fkm", satellite.getApoapsis() / 1000.0);
				ImGui::Text("Periapsis: %.2fkm", satellite.getPeriapsis() / 1000.0);
				ImGui::Text("Eccentricity: %.4f", satellite.getEccentricity());
				ImGui::Text("Semi-major Axis: %.2fkm", satellite.getSemiMajorAxis() / 1000.0);
				ImGui::Text("Argument of Periapsis: %.2f°", glm::degrees(satellite.getArgumentOfPeriapsis()));
				ImGui::Text("Inclination: %.2f°", glm::degrees(satellite.getInclination()));
				ImGui::Text("Longitude of Ascending Node: %.2f°", glm::degrees(satellite.getInclination()));
				ImGui::Text("Orbital Period: %.2fs", satellite.getOrbitalPeriod());
				ImGui::Separator();
				// allow user to show/hide their satellite
				if (satellite.hidden)
				{
					if (ImGui::Button("Show"))
						satellite.hidden = false;
				}
				else
				{
					if (ImGui::Button("Hide"))
						satellite.hidden = true;
				}
				ImGui::SameLine();
				// allow user to destroy satellite, opens a prompt asking the user to confirm
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(255, 0, 0, 255));
				if (ImGui::Button("Destroy"))
				{
					destroyPrompt = true;
					destroyName = satellite.getName();
				}
				ImGui::PopStyleColor();
			}
			ImGui::End();
		}
	}
}

void Simulation::destroyPromptUI()
{
	// popup confirming if user wishes to destroy satellite
	if (destroyPrompt)
	{
		ImGui::OpenPopup("Confirm Destroy");
	}
	if (ImGui::BeginPopup("Confirm Destroy"))
	{
		// warn user of this action
		ImGui::Text("Warning this action is irreversable!");
		if (ImGui::Button("Cancel"))
		{
			destroyPrompt = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(255, 0, 0, 255));
		if (ImGui::Button("Destroy")) // if they confirm remove the satellite
		{
			deleteSatellite(destroyName);
			destroyPrompt = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::PopStyleColor();
		ImGui::EndPopup();
	}
}

void Simulation::physicsUpdate()
{
	// calls physics updates for all earth and satellites
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
	// adds satellite around earth with given launch parameters
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
	// physics update for all satellites in the simulation
	for (int i = 0; i < satellites.size(); i++)
	{
		Satellite& satellite = satellites[i];
		satellite.updatePosition(simTime);
	}
}

void Simulation::deleteSatellite(std::string name)
{
	// removes a satellite based on name matching
	satellites.erase
	(
		std::remove_if
		(
			satellites.begin(),
			satellites.end(),
			// lambda function
			[name](Satellite& satellite) {return satellite.getName() == name;}
		),
		satellites.end()
	);
}

void Simulation::drawSatellites()
{
	// draw call for every satellite loaded into the simulation
	for (int i = 0; i < satellites.size(); i++)
	{
		Satellite& satellite = satellites[i];
		satellite.draw(*iconShader, *textShader, *sunShader, camera, *textLoader, xScale);
	}
}