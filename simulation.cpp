#include "simulation.hpp"

std::vector<Vertex> vertices =
{
	Vertex{ glm::vec3(-0.5f, -0.5f, -0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
	Vertex{ glm::vec3( 0.5f, -0.5f, -0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
	Vertex{ glm::vec3( 0.0f,  0.5f,  0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) }
};
std::vector<GLuint> indices =
{
	0, 1, 2
};

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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	ImGui::GetStyle();
	ImGui::StyleColorsDark();
	io->Fonts->AddFontFromFileTTF("fonts/DejaVuSans.ttf", 15.0f);

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	textLoader = std::make_unique<Text>(16);

	shaderProgram = std::make_unique<Shader>("default.vert", "default.frag");
	meshes.push_back(std::make_unique<UntexturedMesh>(vertices, indices));

	textShader = std::make_unique<Shader>("text.vert", "text.frag");
	iconShader = std::make_unique<Shader>("icon.vert", "icon.frag");

	icon = std::make_unique<CircleIcon>(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "Test", glm::vec3(0.0f, 0.5f, 0.0f));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	//glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	for (int i = 0; i < meshes.size(); i++)
		meshes[i]->draw(*shaderProgram, camera, GL_TRIANGLES);

	icon->draw(*iconShader, *textShader, camera, *textLoader);
}

void Simulation::displayUI() {}
void Simulation::mainMenuUI() {}
void Simulation::controlsUI() {}
void Simulation::SimInfoUI() {}
void Simulation::fpsUI() {}