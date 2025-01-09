#pragma once
#include "Common.h"
//#include "Player.h"
#include "Camera.h"
#include "Light.h"

class Terrain
{
public:
	Terrain();
	~Terrain();
	void Draw(Camera* camera, Light* light);
	FastNoiseLite GetHeightMapNoise() const;
	FastNoiseLite GetBiomeNoise() const;
	int GetRenderDistance() const;
	int GetMapSize() const;
	GLfloat** GetVertices() const;
	float GetVerticesOffset() const;
	bool IsTexturesLoaded() const;
	float GetGrassBiomeValue() const;
	float GetMixBiomeValue() const;
private:
	void GenerateVertices();
	void GenerateIndices();
	// Maybe a function to generate trees? Load models in using Assimp
	void SetupBuffers();
	void SetupTextures();

	const int RENDER_DISTANCE; // Render width of the terrain
	const int MAP_SIZE; // Size of map in x & z space
	const int trianglesPerSquare; // Two triangles per square to form a 1x1 chunk
	const int squaresPerRow; // Amount of squares/chunks per row
	const int trianglesPerTerrain; // Amount of triangles in terrain
	const float VERTICES_OFFSET;
	const float GRASS_BIOME_VALUE;
	const float MIX_BIOME_VALUE;

	enum VAO_IDs { Triangles, Indices, Colours, Textures1, Textures2, NumVAOs = 2 }; // VAO vertex attribute positions in correspondence to vertex attribute type
	GLuint VAOs[NumVAOs]; // VAOs
	enum Buffer_IDs { ArrayBuffer, NumBuffers = 5 }; // Buffer types
	GLuint Buffers[NumBuffers]; // Buffer objects
	bool texturesLoaded;
	
	Shader shaders;

	GLfloat** vertices;
	GLfloat* verticesFlat;
	GLuint** indices;
	GLuint* indicesFlat;

	FastNoiseLite HeightMapNoise; // Perlin noise for height map
	FastNoiseLite BiomeNoise; // Cellular noise for biomes

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	GLfloat shininess;
};