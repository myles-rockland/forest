#include "Monster.h"

Monster::Monster(Terrain* terrain) : movementSpeed(2.0f), terrain(terrain), shaders("shaders/monster.vert", "shaders/monster.frag"), model("media/parasite/Parasite L Starkie.dae")
{
	position = vec3(0.0f, 0.0f, 0.0f);
	forward = vec3(0.0f, 0.0f, 1.0f);
}

Monster::~Monster()
{
	delete terrain;
}

void Monster::Update(Camera* camera, float deltaTime)
{
	// Check if camera is looking at monster
	// Get direction between camera and monster
	vec3 dirToPlayer = camera->GetPosition() - position;
	// Remove y component, normalise
	dirToPlayer.y = 0.0f;
	dirToPlayer = normalize(dirToPlayer);
	// Find angle difference between camera front and dirToPlayer vectors
	float dotProduct = dot(normalize(camera->GetFront()), -dirToPlayer);
	dotProduct = clamp(dotProduct, -1.0f, 1.0f);
	float angle = acos(dotProduct);
	// If magnitude of angle greater than or equal to 45 degrees, monster is outside player sight
	if (abs(angle) >= radians(45.0f))
	{
		// Move towards player
		position += dirToPlayer * movementSpeed * deltaTime;
	}
}

void Monster::Draw(Camera* camera, Light* light)
{
	shaders.use();

	// Model matrix
	mat4 model = mat4(1.0f);
	model = translate(model, position);
	model = scale(model, vec3(0.007f));
	// Rotate model to look at player/camera
	//model = rotate(model, (float)glfwGetTime(), vec3(0.0f, 1.0f, 0.0f));

	// Find direction towards player/camera
	vec3 dirToPlayer = camera->GetPosition() - position;
	// Remove y component, normalise
	dirToPlayer.y = 0.0f;
	dirToPlayer = normalize(dirToPlayer);
	// Find angle difference between front and dirToPlayer vectors
	float dotProduct = dot(normalize(forward), dirToPlayer);
	dotProduct = clamp(dotProduct, -1.0f, 1.0f);
	float angle = acos(dotProduct);
	// Rotate model by this angle
	cout << "Rotating by " << angle << " radians" << endl;
	model = rotate(model, angle, vec3(0.0f, 1.0f, 0.0f));

	// View matrix
	mat4 view = lookAt(camera->GetPosition(), camera->GetPosition() + camera->GetFront(), camera->GetUp());

	// Projection matrix
	mat4 projection = perspective(radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

	// MVP matrix
	mat4 mvp = projection * view * model;
	shaders.setMat4("mvpIn", mvp); //Setting of uniform with Shader class

	this->model.Draw(shaders);
}