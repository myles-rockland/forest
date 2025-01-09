#pragma once
#include "Common.h"

// Custom classes
#include "Signature.h"
#include "Light.h"
#include "Terrain.h"
#include "Camera.h"
#include "Player.h"
#include "Collectable.h"
#include "Monster.h"
#include "Tree.h"
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
	// Initialise game objects
	void Initialise();
	// Processes user input on a particular window
	void ProcessUserInput(GLFWwindow* WindowIn);
	// Update all game objects
	void Update();
	// Render all game objects
	void Draw();

	static Game* instance;
	irrklang::ISoundEngine* soundEngine;
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
	Monster* monster;
	Collectable* collectables[5];
	Tree* trees[10];
	Signature* signature;
};

