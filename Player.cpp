#include "Player.h"

Player::Player(Terrain* terrain, Camera* camera, irrklang::ISoundEngine* engine) : terrain(terrain)
{
	// Spawn the player in middle of terrain
	int middleIndex = terrain->GetMapSize() / 2 - terrain->GetRenderDistance() / 2;
    GLfloat** vertices = terrain->GetVertices();
    GLfloat* middleVertex = vertices[middleIndex];
    float x = middleVertex[0];
    float y = middleVertex[1];
    float z = middleVertex[2];
	position = vec3(x, y, z);

    movementSpeed = 1.0f;

	this->camera = camera;

    walkingSound = engine->play3D("./audio/walking_on_leaves.ogg", irrklang::vec3df(position.x, position.y, position.z), true, true, true);
}

Player::~Player()
{
	delete camera;
    if (walkingSound)
        walkingSound->drop();
}

void Player::ProcessInput(GLFWwindow* windowIn, float deltaTime, irrklang::ISoundEngine* engine)
{
	// Move player
    const float speed = movementSpeed * deltaTime;
    // WASD controls
    if (glfwGetKey(windowIn, GLFW_KEY_W) == GLFW_PRESS)
    {
        vec3 dir = camera->GetFront();
        dir.y = 0.0f;
        dir = normalize(dir);
        position += speed * dir;
        // Set sound position
        walkingSound->setPosition(irrklang::vec3df(position.x, position.y, position.z));
        // Play walking sound
        walkingSound->setIsPaused(false);
    }
    if (glfwGetKey(windowIn, GLFW_KEY_S) == GLFW_PRESS)
    {
        vec3 dir = camera->GetFront();
        dir.y = 0.0f;
        dir = normalize(dir);
        position -= speed * dir;
        // Set sound position
        walkingSound->setPosition(irrklang::vec3df(position.x, position.y, position.z));
        // Play walking sound
        walkingSound->setIsPaused(false);
    }
    if (glfwGetKey(windowIn, GLFW_KEY_A) == GLFW_PRESS)
    {
        position -= normalize(cross(camera->GetFront(), camera->GetUp())) * speed;
        // Set sound position
        walkingSound->setPosition(irrklang::vec3df(position.x, position.y, position.z));
        // Play walking sound
        walkingSound->setIsPaused(false);
    }
    if (glfwGetKey(windowIn, GLFW_KEY_D) == GLFW_PRESS)
    {
        position += normalize(cross(camera->GetFront(), camera->GetUp())) * speed;
        // Set sound position
        walkingSound->setPosition(irrklang::vec3df(position.x, position.y, position.z));
        // Play walking sound
        walkingSound->setIsPaused(false);
    }
    if (glfwGetKey(windowIn, GLFW_KEY_W) != GLFW_PRESS
        && glfwGetKey(windowIn, GLFW_KEY_A) != GLFW_PRESS
        && glfwGetKey(windowIn, GLFW_KEY_S) != GLFW_PRESS
        && glfwGetKey(windowIn, GLFW_KEY_D) != GLFW_PRESS)
    {
        walkingSound->setIsPaused(true);
    }

    // Clamp player x position between 0 and terrain max
    position.x = clamp(position.x, 0.0f, terrain->GetVerticesOffset() * terrain->GetRenderDistance());
    // Clamp player z between 0 and terrain max
    position.z = clamp(position.z, 0.0f, terrain->GetVerticesOffset() * terrain->GetRenderDistance());

    // Update player y position based on terrain heightmap
    FastNoiseLite heightMapNoise = terrain->GetHeightMapNoise();
    position.y = heightMapNoise.GetNoise(position.x / terrain->GetVerticesOffset(), position.z / terrain->GetVerticesOffset()); // Need to divide by vertices offset due to how terrain is generated

	// Update camera position
    camera->SetPosition(position + vec3(0.0f, 0.7f, 0.0f));
    vec3 cameraPosition = camera->GetPosition();
}

Camera* Player::GetCamera() const { return camera; }
vec3 Player::GetPosition() const { return position; }