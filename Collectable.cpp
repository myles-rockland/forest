#include "Collectable.h"

Collectable::Collectable() : player(nullptr), shaders("shaders/basic.vert", "shaders/basic.frag"), model("media/folded_wrinkled_paper/scene.gltf") {}
Collectable::Collectable(Terrain* terrain, Player* player) : shaders("shaders/basic.vert", "shaders/basic.frag"), model("media/folded_wrinkled_paper/scene.gltf")
{
	this->player = player;

	//// Create generator for use in distribution
	//std::random_device rd;
	//std::mt19937 generator(rd());

	//// Create distribution 
	//float max = terrain->GetVerticesOffset() * terrain->GetRenderDistance();
	//std::uniform_real_distribution<float> distribution(0.0f, max);

	//// Assign random position using distribution
	//float x = distribution(generator);
	//float z = distribution(generator);
	//
	//// Update monster y position based on terrain heightmap
	//FastNoiseLite heightMapNoise = terrain->GetHeightMapNoise();
	//float y = heightMapNoise.GetNoise(x / terrain->GetVerticesOffset(), z / terrain->GetVerticesOffset());
	//y += 0.3f;
	//
	//// Set position
	//position = vec3(x, y, z);
	
	// Generate two random numbers within terrain render distance
	int x = rand() % terrain->GetRenderDistance();
	int y = rand() % terrain->GetRenderDistance();

	// Get terrain biome noise
	FastNoiseLite biomeNoise = terrain->GetBiomeNoise();

	float biomeValue = biomeNoise.GetNoise((float)x, (float)y);

	// If "vertex" is not within Dirt terrain, generate a new random "vertex"
	while (biomeValue < terrain->GetMixBiomeValue() + 0.2f) // Adding 0.2f to increase chance that it is within Dirt biome. Anything >= -0.45 should be good
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
	position = vec3(vertex[0], vertex[1] + 0.3f, vertex[2]);
	
	// Define radius using terrain
	radius = terrain->GetVerticesOffset() * 10;

	// Default collected to false
	collected = false;
}

Collectable::~Collectable()
{
	delete player;
}

void Collectable::Update()
{
	if (!collected)
	{
		// Get distance between this and player
		float dist = distance(position, player->GetPosition());
		// If player is within radius, this collectable is collected
		if (dist < radius)
		{
			collected = true;
		}
	}
}

void Collectable::Draw(Camera* camera)
{
	shaders.use();

	// Model matrix
	mat4 model = mat4(1.0f);
	model = translate(model, position);
	model = translate(model, vec3(0.0f, sin(glfwGetTime()) * 0.05f, 0.0f)); // Translate y position using sin() to make it move up and down
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

bool Collectable::IsCollected() const { return collected; }