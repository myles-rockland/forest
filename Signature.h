#pragma once
#include "Common.h"
#include "Camera.h"

using namespace glm;

class Signature
{
public:
	Signature();
	void Draw();
	bool IsTexturesLoaded() const;
private:
	void SetupBuffers();
	void SetupTextures();

	vec3 position;

	float vertices[20];
	unsigned int indices[6];

	enum VAO_IDs { Triangles, Indices, Colours, Textures, NumVAOs = 2 }; // VAO vertex attribute positions in correspondence to vertex attribute type
	GLuint VAOs[NumVAOs]; // VAOs
	enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 }; // Buffer types
	GLuint Buffers[NumBuffers]; // Buffer objects

	bool texturesLoaded;

	Shader shaders;
};

