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
public:
	Terrain();
	~Terrain();
	void Draw(Camera* camera, Light* light);
private:
	void GenerateVertices();
	void GenerateIndices();
	void SetupBuffers();

	const int RENDER_DISTANCE; // Render width of the terrain
	const int MAP_SIZE; // Size of map in x & z space
	const int trianglesPerSquare; // Two triangles per square to form a 1x1 chunk
	const int squaresPerRow; // Amount of squares/chunks per row
	const int trianglesPerTerrain; // Amount of triangles in terrain

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