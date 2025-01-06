#pragma once
#include "Common.h"
#include "Camera.h"
#include "Terrain.h"

class Player
{
public:
	Player(Terrain* terrain, Camera* camera);
	~Player();
	void ProcessInput(GLFWwindow* windowIn, float deltaTime);
	Camera* GetCamera() const;
	vec3 GetPosition() const;

private:
	vec3 position;
	float movementSpeed;
	Terrain* terrain;

	Camera* camera;
};

