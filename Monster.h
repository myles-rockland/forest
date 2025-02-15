#pragma once
#include "Common.h"
#include "Terrain.h"
class Monster
{
public:
	Monster(Terrain* terrain, irrklang::ISoundEngine* engine);
	~Monster();
	void Update(Camera* camera, float deltaTime);
	void Draw(Camera* camera, Light* light); // Might not need light
	bool GetCaughtPlayer() const;
private:
	vec3 position;
	vec3 forward;
	float movementSpeed;
	float radius;
	bool caughtPlayer;
	Terrain* terrain;
	Shader shaders;
	Model model;
	irrklang::ISound* breathingSound;
};

