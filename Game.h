#pragma once
#include "Common.h"

// Custom classes
#include "Light.h"
#include "Terrain.h"
#include "Camera.h"
#include "Player.h"
class Game
{
public:
	Game();
	~Game();
	void Run();
private:
	// Called on window resize
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	// Called on mouse movement
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	// Processes user input on a particular window
	void ProcessUserInput(GLFWwindow* WindowIn);
	// Initialises GLFW
	int InitialiseGLFW();

	static Game* instance;
	GLFWwindow* window;
	bool isRunning;
	//Time
	//Time change
	float deltaTime;
	//Last value of time change
	float lastFrame;

	// Game objects
	Camera* camera;
	Terrain* terrain;
	Player* player;
	Light* light;
};

