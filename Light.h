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

class Light
{
	Light();
public:
	vec3 GetPosition() const;
	vec3 GetAmbient() const;
	vec3 GetDiffuse() const;
	vec3 GetSpecular() const;
private:
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

