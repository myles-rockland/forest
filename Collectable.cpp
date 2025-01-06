#include "Collectable.h"

Collectable::Collectable(Terrain* terrain, Player* player, vec3 position) : shaders("shaders/basic.vert", "shaders/basic.frag"), model("media/folded_wrinkled_paper/scene.gltf")
{
	this->player = player;
	this->position = position;
	
	// Define radius using terrain
	radius = terrain->GetVerticesOffset() * 10;
}

Collectable::~Collectable()
{
	delete player;
}

bool Collectable::CheckCollected()
{
	// Get distance between this and player
	float dist = distance(position, player->GetPosition());
	if (dist < radius)
	{
		cout << "Collected" << endl;
		return true;
	}
	else
	{
		return false;
	}
}

void Collectable::Draw(Camera* camera)
{
	shaders.use();

	// Model matrix
	mat4 model = mat4(1.0f);
	model = translate(model, position); // Add sin(glfwGetTime()) to position.y?
	model = scale(model, vec3(0.01f));
	model = rotate(model, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));

	// Rotate model over time for animation
	model = rotate(model, (float)glfwGetTime(), vec3(1.0f, 0.0f, 0.0f));

	// View matrix
	mat4 view = lookAt(camera->GetPosition(), camera->GetPosition() + camera->GetFront(), camera->GetUp());

	// Projection matrix
	mat4 projection = perspective(radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

	// MVP matrix
	mat4 mvp = projection * view * model;
	shaders.setMat4("mvpIn", mvp);

	this->model.Draw(shaders);
}