#pragma once
#include "Common.h"
#include "Player.h"
#include <random>

using namespace glm;

class Collectable
{
public:
	Collectable();
	Collectable(Terrain* terrain, Player* player);
	~Collectable();
	void Update();
	bool IsCollected() const;
	void Draw(Camera* camera);
private:
	vec3 position;
	float radius;
	Player* player;
	bool collected;

	Shader shaders;
	Model model;
};