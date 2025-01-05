#pragma once
#include "Common.h"
#include "Terrain.h"
class Monster
{
public:
	Monster(Terrain* terrain);
	~Monster();
	void Draw(Camera* camera, Light* light); // Might not need light
private:
	vec3 position;
	float movementSpeed;
	Terrain* terrain;
	Shader shaders;
	Model model;
};

