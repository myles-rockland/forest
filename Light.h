#pragma once
#include "Common.h"
#include "Camera.h"

class Terrain;

class Light
{
public:
	Light(Terrain* terrain);
	vec3 GetPosition() const;
	vec3 GetAmbient() const;
	vec3 GetDiffuse() const;
	vec3 GetSpecular() const;
	void Update(Terrain* terrain);
	void Draw(Camera* camera);
private:
	void SetupBuffers();
	vec3 position;
	vec3 origin;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	enum VAO_IDs { Triangles, Indices, Colours, Textures, NumVAOs = 2 }; // VAO vertex attribute positions in correspondence to vertex attribute type
	GLuint VAOs[NumVAOs]; // VAOs
	enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 }; // Buffer types
	GLuint Buffers[NumBuffers]; // Buffer objects

	Shader shaders; //ShaderInfo shaders[3];
	//GLuint shaderProgram;

	float vertices[3 * 6 * 6]; // 3 position components, 6 vertices in a face, 6 faces

	float radius;
	float movementSpeed;
};

