#pragma once
#include "Common.h"
#include "Player.h"

using namespace glm;

class Collectable
{
public:
	Collectable(Terrain* terrain, Player* player, vec3 position);
	~Collectable();
	bool CheckCollected();
	void Draw(Camera* camera);
private:
	vec3 position;
	float radius;
	Player* player;

	Shader shaders;
	Model model;
};