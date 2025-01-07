#pragma once
#include "Common.h"
#include "Camera.h"
#include "Terrain.h"

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

