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

#include "Constants.h"

using namespace glm;

class Camera
{
public:
	Camera();
	vec3 GetPosition() const;
	vec3 GetFront() const;
	vec3 GetUp() const;
	void MouseCallback(GLFWwindow* window, double xpos, double ypos);
	void ProcessUserInput(GLFWwindow* windowIn, float deltaTime);

private:
	//Transformations
	vec3 position; // Relative position within world space
	vec3 front; // Forwards direction of travel
	vec3 up; // Up direction within world space
	float yaw; // Camera sideways rotation
	float pitch; // Camera vertical rotation
	const float sensitivity; // Mouse sensitivity
	const float movementSpeed;

	bool mouseFirstEntry; // Determines if first entry of mouse into window

	//Positions of camera from given last frame
	float lastXPos;
	float lastYPos;
};