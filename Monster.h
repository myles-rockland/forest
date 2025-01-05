#pragma once
#include "Common.h"
#include "Terrain.h"
class Monster
{
public:
	Monster(Terrain* terrain);
	~Monster();
	void Update(Camera* camera, float deltaTime);
	void Draw(Camera* camera, Light* light); // Might not need light
private:
	vec3 position;
	vec3 forward;
	float movementSpeed;
	Terrain* terrain;
	Shader shaders;
	Model model;
};

