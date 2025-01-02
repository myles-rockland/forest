#include "Camera.h"

Camera::Camera()
{
	position = vec3(0.0f, 0.0f, 3.0f);
	front = vec3(0.0f, 0.0f, -1.0f);
	up = vec3(0.0f, 1.0f, 0.0f);

	yaw = -90.0f;
	pitch = 0.0f;

	mouseFirstEntry = true;
	lastXPos = (float)WINDOW_WIDTH / 2.0f;
	lastYPos = (float)WINDOW_HEIGHT / 2.0f;
}

vec3 Camera::GetPosition() const { return position; }
vec3 Camera::GetFront() const { return front; }
vec3 Camera::GetUp() const { return up; }