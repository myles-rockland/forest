#pragma once
#pragma once
// STD
#include <iostream>

// GLEW
#include <GL/glew.h>

// GLM
#include "glm/ext/vector_float3.hpp"
#include <glm/gtc/type_ptr.hpp> // GLM: access to the value_ptr

// GLFW
#include <GLFW/glfw3.h>

// Shaders
#include "shaders/LoadShaders.h"

#include "Constants.h"
#include "Camera.h"
#include "Terrain.h"

class Player
{
public:
	Player(Terrain* terrain, Camera* camera);
	~Player();
	void ProcessInput(GLFWwindow* windowIn, float deltaTime);
	Camera* GetCamera() const;

private:
	vec3 position;
	float movementSpeed;
	Terrain* terrain;

	Camera* camera;
};

