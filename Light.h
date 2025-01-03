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
public:
	Light();
	vec3 GetPosition() const;
	vec3 GetAmbient() const;
	vec3 GetDiffuse() const;
	vec3 GetSpecular() const;
	void Draw(Camera* camera);
private:
	void SetupBuffers();
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	enum VAO_IDs { Triangles, Indices, Colours, Textures, NumVAOs = 2 }; // VAO vertex attribute positions in correspondence to vertex attribute type
	GLuint VAOs[NumVAOs]; // VAOs
	enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 }; // Buffer types
	GLuint Buffers[NumBuffers]; // Buffer objects

	ShaderInfo shaders[3];
	GLuint shaderProgram;

	float vertices[3 * 6 * 6]; // 3 position components, 6 vertices in a face, 6 faces
};

