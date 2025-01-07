#pragma once
#include "Common.h"
#include "Terrain.h"
#include "Player.h"

using namespace glm;

class Tree
{
public:
	Tree();
	Tree(Terrain* terrain);
	~Tree();
	void Draw(Camera* camera);
private:
	vec3 position;

	Shader shaders;
	Model model;
};

