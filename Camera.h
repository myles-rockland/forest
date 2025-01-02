#pragma once
#include "glm/ext/vector_float3.hpp"
#include "Constants.h"

using namespace glm;

class Camera
{
	Camera();
public:
	vec3 GetPosition() const;
	vec3 GetFront() const;
	vec3 GetUp() const;

private:
	//Transformations
	vec3 position; // Relative position within world space
	vec3 front; // Forwards direction of travel
	vec3 up; // Up direction within world space
	float yaw; // Camera sideways rotation
	float pitch; // Camera vertical rotation

	bool mouseFirstEntry; // Determines if first entry of mouse into window

	//Positions of camera from given last frame
	float lastXPos;
	float lastYPos;
};