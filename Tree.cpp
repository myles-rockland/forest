#include "Tree.h"

Tree::Tree() : shaders("shaders/basic.vert", "shaders/basic.frag"), model("media/pine_tree/scene.gltf") {}

Tree::Tree(Terrain* terrain) : shaders("shaders/basic.vert", "shaders/basic.frag"), model("media/pine_tree/scene.gltf") 
{
	// Create generator for use in distribution
	std::random_device rd;
	std::mt19937 generator(rd());

	// Create distribution 
	float max = terrain->GetVerticesOffset() * terrain->GetRenderDistance();
	std::uniform_real_distribution<float> distribution(0.0f, max);

	// Assign random position using distribution
	float x = distribution(generator);
	float z = distribution(generator);

	// Update monster y position based on terrain heightmap
	FastNoiseLite heightMapNoise = terrain->GetHeightMapNoise();
	float y = heightMapNoise.GetNoise(x / terrain->GetVerticesOffset(), z / terrain->GetVerticesOffset());
	y -= 0.3f;

	// Set position
	position = vec3(x, y, z);
}

Tree::~Tree() {}

void Tree::Draw(Camera* camera)
{
	shaders.use();

	// Model matrix
	mat4 model = mat4(1.0f);
	model = translate(model, position);
	//model = scale(model, vec3(1.0f));
	model = rotate(model, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));

	// View matrix
	mat4 view = lookAt(camera->GetPosition(), camera->GetPosition() + camera->GetFront(), camera->GetUp());

	// Projection matrix
	mat4 projection = perspective(radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

	// MVP matrix
	mat4 mvp = projection * view * model;
	shaders.setMat4("mvpIn", mvp);

	this->model.Draw(shaders);
}