#pragma once
#include "Common.h"
#include "Player.h"

using namespace glm;

class Collectable
{
public:
	Collectable();
	Collectable(Terrain* terrain, Player* player);
	~Collectable();
	void Update(irrklang::ISoundEngine* engine);
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