#include "Monster.h"

Monster::Monster(Terrain* terrain) : movementSpeed(1.0f), terrain(terrain), shaders("shaders/monster.vert", "shaders/monster.frag"), model("media/parasite/Parasite L Starkie.dae")
{
	position = vec3(0.0f, 0.0f, 0.0f);
}

Monster::~Monster()
{
	delete terrain;
}

void Monster::Draw(Camera* camera, Light* light)
{
	// Model matrix
	mat4 model = mat4(1.0f);
	model = translate(model, position);

	// View matrix
	mat4 view = lookAt(camera->GetPosition(), camera->GetPosition() + camera->GetFront(), camera->GetUp());

	// Projection matrix
	mat4 projection = perspective(radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

	// MVP matrix
	mat4 mvp = projection * view * model;
	shaders.setMat4("mvpIn", mvp); //Setting of uniform with Shader class

	shaders.use();
	this->model.Draw(shaders);
}