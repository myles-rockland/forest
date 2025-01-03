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

// Shaders and Noise
#include "shaders/LoadShaders.h"
#include "FastNoiseLite.h"

// Constants
#include "Constants.h"

// Custom classes
#include "Light.h"
#include "Terrain.h"
#include "Camera.h"

// Called on window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// Called on mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
// Processes user input on a particular window
void ProcessUserInput(GLFWwindow* WindowIn);

int InitialiseGLFW();

// GLFW Window
GLFWwindow* window;