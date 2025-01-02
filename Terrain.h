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

#include "Constants.h"
#include "Camera.h"
#include "Light.h"

class Terrain
{
	Terrain();
	~Terrain();
	void GenerateVertices();
	void GenerateIndices();
	void SetupBuffers();
	void Draw(Camera camera, Light light);

	const int RENDER_DISTANCE = 128; // Render width of the terrain
	const int MAP_SIZE = RENDER_DISTANCE * RENDER_DISTANCE; // Size of map in x & z space

	const int trianglesPerSquare = 2; // Two triangles per square to form a 1x1 chunk
	const int squaresPerRow = RENDER_DISTANCE - 1; // Amount of squares/chunks per row
	const int trianglesPerTerrain = squaresPerRow * squaresPerRow * trianglesPerSquare; // Amount of triangles in terrain

	enum VAO_IDs { Triangles, Indices, Colours, Textures, NumVAOs = 2 }; // VAO vertex attribute positions in correspondence to vertex attribute type
	GLuint VAOs[NumVAOs]; // VAOs
	enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 }; // Buffer types
	GLuint Buffers[NumBuffers]; // Buffer objects

	ShaderInfo shaders[3];
	GLuint shaderProgram;

	GLfloat** vertices;
	GLfloat* verticesFlat;
	GLuint** indices;
	GLuint* indicesFlat;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	GLfloat shininess;
};