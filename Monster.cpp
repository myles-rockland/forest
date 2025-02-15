#include "Monster.h"

Monster::Monster(Terrain* terrain, irrklang::ISoundEngine* engine) : movementSpeed(2.0f), terrain(terrain), shaders("shaders/basic.vert", "shaders/basic.frag"), model("media/parasite/Parasite L Starkie.dae")
{
	// Spawn the player in middle of terrain
	int middleIndex = terrain->GetMapSize() - 1;
	GLfloat** vertices = terrain->GetVertices();
	GLfloat* middleVertex = vertices[middleIndex];
	float x = middleVertex[0];
	float y = middleVertex[1];
	float z = middleVertex[2];
	position = vec3(x, y, z);

	//position = vec3(0.0f, 0.0f, 0.0f);
	forward = vec3(0.0f, 0.0f, 1.0f);
	radius = terrain->GetVerticesOffset() * 15;
	caughtPlayer = false;
	breathingSound = engine->play3D("./audio/zombie_breathing.ogg", irrklang::vec3df(position.x, position.y, position.z), true, false, true);
}

Monster::~Monster()
{
	delete terrain;
	if (breathingSound)
	{
		breathingSound->drop();
	}
}

void Monster::Update(Camera* camera, float deltaTime)
{
	// Check if camera is looking at monster
	// Get direction between camera and monster
	vec3 dirToPlayer = camera->GetPosition() - position;
	float distToPlayer = distance(camera->GetPosition(), position);
	// Remove y component, normalise
	dirToPlayer.y = 0.0f;
	dirToPlayer = normalize(dirToPlayer);
	// Find angle difference between camera front and dirToPlayer vectors
	float dotProduct = dot(normalize(camera->GetFront()), -dirToPlayer); // Minus as it needs to get the smaller angle
	dotProduct = clamp(dotProduct, -1.0f, 1.0f);
	float angle = acos(dotProduct);
	// If magnitude of angle greater than or equal to 45 degrees, monster is outside player sight
	if (abs(angle) >= radians(45.0f))
	{
		// Move towards player
		position += dirToPlayer * movementSpeed * deltaTime;

		// Clamp monster x position between 0 and terrain max
		position.x = clamp(position.x, 0.0f, terrain->GetVerticesOffset() * terrain->GetRenderDistance());
		// Clamp monster z between 0 and terrain max
		position.z = clamp(position.z, 0.0f, terrain->GetVerticesOffset() * terrain->GetRenderDistance());

		// Update monster y position based on terrain heightmap
		FastNoiseLite heightMapNoise = terrain->GetHeightMapNoise();
		position.y = heightMapNoise.GetNoise(position.x / terrain->GetVerticesOffset(), position.z / terrain->GetVerticesOffset()); // Need to divide by vertices offset due to how terrain is generated
	}

	// Update sound position
	breathingSound->setPosition(irrklang::vec3df(position.x, position.y, position.z));

	// Get distance between this and player
	float dist = distance(position, camera->GetPosition());
	// If player is within radius, the player was caught, i.e. game over
	if (dist < radius)
	{
		caughtPlayer = true;
		cout << "You were caught! Game over!" << endl;
	}
}

void Monster::Draw(Camera* camera, Light* light)
{
	shaders.use();

	// Model matrix
	mat4 model = mat4(1.0f);
	model = translate(model, position);
	model = scale(model, vec3(0.005f));

	// Rotate model to look at player/camera
	// Find direction towards player/camera
	vec3 dirToPlayer = camera->GetPosition() - position;
	// Remove y component, normalise
	dirToPlayer.y = 0.0f;
	dirToPlayer = normalize(dirToPlayer);
	// Find angle difference between front and dirToPlayer vectors
	vec3 crossProduct = cross(normalize(forward), dirToPlayer);
	float dotProduct = dot(normalize(forward), dirToPlayer);
	dotProduct = clamp(dotProduct, -1.0f, 1.0f);
	float angle = acos(dotProduct);

	// Determine the rotation direction 
	if (crossProduct.y < 0.0f) 
	{ 
		angle = -angle; 
	}
	// Rotate model by this angle
	//cout << "Rotating by " << angle << " radians" << endl;
	model = rotate(model, angle, vec3(0.0f, 1.0f, 0.0f));

	// View matrix
	mat4 view = lookAt(camera->GetPosition(), camera->GetPosition() + camera->GetFront(), camera->GetUp());

	// Projection matrix
	mat4 projection = perspective(radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

	// MVP matrix
	mat4 mvp = projection * view * model;
	shaders.setMat4("mvpIn", mvp);

	this->model.Draw(shaders);
}

bool Monster::GetCaughtPlayer() const { return caughtPlayer; }