#include "Tree.h"

Tree::Tree() : shaders("shaders/basic.vert", "shaders/basic.frag"), model("media/pine_tree/scene.gltf") {}

Tree::Tree(Terrain* terrain) : shaders("shaders/basic.vert", "shaders/basic.frag"), model("media/pine_tree/scene.gltf")
{
	// Generate two random numbers within terrain render distance
	int x = rand() % terrain->GetRenderDistance();
	int y = rand() % terrain->GetRenderDistance();

	// Get terrain biome noise
	FastNoiseLite biomeNoise = terrain->GetBiomeNoise();

	float biomeValue = biomeNoise.GetNoise((float)x, (float)y);

	// If "vertex" is not within Grass terrain, generate a new random "vertex"
	while (biomeValue > terrain->GetGrassBiomeValue() - 0.2f) // Taking 0.2f to increase chance that it is within grass biome. Anything <= -0.95 should be good
	{
		// Generate two random numbers within terrain render distance
		x = rand() % terrain->GetRenderDistance();
		y = rand() % terrain->GetRenderDistance();

		biomeValue = biomeNoise.GetNoise((float)x, (float)y);
	}

	// Get a terrain vertex 
	GLfloat** vertices = terrain->GetVertices();
	GLfloat* vertex = vertices[x * terrain->GetRenderDistance() + y];

	// Set tree position based on terrain vertex
	position = vec3(vertex[0], vertex[1] - 0.3f, vertex[2]);
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