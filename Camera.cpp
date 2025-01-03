#include "Camera.h"

Camera::Camera() : sensitivity(0.025f), movementSpeed(1.0f)
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

void Camera::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    //Initially no last positions, so sets last positions to current positions
    if (mouseFirstEntry)
    {
        lastXPos = (float)xpos;
        lastYPos = (float)ypos;
        mouseFirstEntry = false;
    }

    //Sets values for change in position since last frame to current frame
    float xOffset = (float)xpos - lastXPos;
    float yOffset = lastYPos - (float)ypos;

    //Sets last positions to current positions for next frame
    lastXPos = (float)xpos;
    lastYPos = (float)ypos;

    //Moderates the change in position based on sensitivity value
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    //Adjusts yaw & pitch values against changes in positions
    yaw += xOffset;
    pitch += yOffset;

    //Prevents turning up & down beyond 90 degrees to look backwards
    if (pitch > 89.0f)
    {
        pitch = 89.0f;
    }
    else if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }

    //Modification of direction vector based on mouse turning
    vec3 direction;
    direction.x = cos(radians(yaw)) * cos(radians(pitch));
    direction.y = sin(radians(pitch));
    direction.z = sin(radians(yaw)) * cos(radians(pitch));
    front = normalize(direction);
}

void Camera::ProcessUserInput(GLFWwindow* windowIn, float deltaTime)
{
    //Extent to which to move in one instance
    const float speed = movementSpeed * deltaTime;
    //WASD controls
    if (glfwGetKey(windowIn, GLFW_KEY_W) == GLFW_PRESS)
    {
        position += speed * front;
    }
    if (glfwGetKey(windowIn, GLFW_KEY_S) == GLFW_PRESS)
    {
        position -= speed * front;
    }
    if (glfwGetKey(windowIn, GLFW_KEY_A) == GLFW_PRESS)
    {
        position -= normalize(cross(front, up)) * speed;
    }
    if (glfwGetKey(windowIn, GLFW_KEY_D) == GLFW_PRESS)
    {
        position += normalize(cross(front, up)) * speed;
    }
}

vec3 Camera::GetPosition() const { return position; }
vec3 Camera::GetFront() const { return front; }
vec3 Camera::GetUp() const { return up; }