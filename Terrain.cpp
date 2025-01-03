#include "Terrain.h"

using namespace glm;

Terrain::Terrain() : vertices(nullptr), indices(nullptr), RENDER_DISTANCE(128), MAP_SIZE(RENDER_DISTANCE * RENDER_DISTANCE), trianglesPerSquare(2), squaresPerRow(RENDER_DISTANCE-1), trianglesPerTerrain(squaresPerRow * squaresPerRow * trianglesPerSquare)
{
	shaders[0] = { GL_VERTEX_SHADER, "shaders/terrain.vert" };
	shaders[1] = { GL_FRAGMENT_SHADER, "shaders/terrain.frag" };
	shaders[2] = { GL_NONE, NULL };

	shaderProgram = LoadShaders(shaders);

    GenerateVertices();
    GenerateIndices();

    ambient = vec3(1.0f);
    diffuse = vec3(1.0f);
    specular = vec3(0.1f);
    shininess = 2.0f;

    SetupBuffers();
}

Terrain::~Terrain()
{
    // Delete the arrays
    for (int i = 0; i < MAP_SIZE; i++) {
        delete[] vertices[i]; // Free each row
    }
    delete[] vertices;
    delete[] verticesFlat;
    for (int i = 0; i < MAP_SIZE; i++) {
        delete[] indices[i]; // Free each row
    }
    delete[] indices;
    delete[] indicesFlat;
}

void Terrain::GenerateVertices()
{
    vertices = new GLfloat * [MAP_SIZE];
    for (int i = 0; i < MAP_SIZE; i++)
    {
        vertices[i] = new GLfloat[9]; // 3 position components, 3 colour components, 3 normal components
        vertices[i][0] = 0.0f;
        vertices[i][1] = 0.0f;
        vertices[i][2] = 0.0f;
        vertices[i][3] = 0.0f;
        vertices[i][4] = 0.0f;
        vertices[i][5] = 0.0f;
        vertices[i][6] = 0.0f;
        vertices[i][7] = 0.0f;
        vertices[i][8] = 0.0f;
    }

    //Positions to start drawing from
    float drawingStartPosition = 1.0f;
    float columnVerticesOffset = drawingStartPosition;
    float rowVerticesOffset = drawingStartPosition;

    int rowIndex = 0;
    for (int i = 0; i < MAP_SIZE; i++)
    {
        //Generation of x & z vertices for horizontal plane
        vertices[i][0] = columnVerticesOffset;
        vertices[i][2] = rowVerticesOffset;

        //Shifts x position across for next triangle along grid
        columnVerticesOffset -= 0.0625f;

        //Indexing of each chunk within row
        rowIndex++;
        //True when all triangles of the current row have been generated
        if (rowIndex == RENDER_DISTANCE)
        {
            //Resets for next row of triangles
            rowIndex = 0;
            //Resets x position for next row of triangles
            columnVerticesOffset = drawingStartPosition;
            //Shifts y position
            rowVerticesOffset -= 0.0625f;
        }
    }

    // Perlin noise for heightmap
    FastNoiseLite TerrainNoise;
    //Setting noise type to Perlin
    TerrainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    //Sets the noise scale
    TerrainNoise.SetFrequency(0.05f);
    //Generates a random seed between integers 0 & 100
    srand(time(0)); // Seeding the rand() function using time
    int terrainSeed = rand() % 100;
    //Sets seed for noise
    TerrainNoise.SetSeed(terrainSeed);

    //Biome noise
    FastNoiseLite BiomeNoise;
    BiomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    BiomeNoise.SetFrequency(0.05f);
    srand(time(0)); // Seeding the rand() function using time
    int biomeSeed = rand() % 100;
    TerrainNoise.SetSeed(biomeSeed);

    //Terrain vertex index
    int i = 0;
    //Using x & y nested for loop in order to apply noise 2-dimensionally
    for (int y = 0; y < RENDER_DISTANCE; y++)
    {
        for (int x = 0; x < RENDER_DISTANCE; x++)
        {
            int index = y * RENDER_DISTANCE + x;
            if (index < MAP_SIZE)
            {
                //Setting of height from 2D noise value at respective x & y coordinate
                vertices[index][1] = TerrainNoise.GetNoise((float)x, (float)y);

                //Retrieval of biome to set
                float biomeValue = BiomeNoise.GetNoise((float)x, (float)y);

                if (biomeValue <= -0.75f) //Plains
                {
                    vertices[index][3] = 0.0f;
                    vertices[index][4] = 0.75f;
                    vertices[index][5] = 0.25f;
                }
                else //Desert
                {
                    vertices[index][3] = 1.0f;
                    vertices[index][4] = 1.0f;
                    vertices[index][5] = 0.5f;
                }
            }
        }
    }
}

void Terrain::GenerateIndices()
{
    //Generation of height map indices
    indices = new GLuint * [trianglesPerTerrain];
    for (int i = 0; i < trianglesPerTerrain; i++)
    {
        indices[i] = new GLuint[3];
        indices[i][0] = 0;
        indices[i][1] = 0;
        indices[i][2] = 0;
    }

    //Positions to start mapping indices from
    int columnIndicesOffset = 0;
    int rowIndicesOffset = 0;

    //Generation of map indices in the form of chunks (1x1 right angle triangle squares)
    int rowIndex = 0;
    for (int i = 0; i < trianglesPerTerrain - 1; i += 2)
    {
        indices[i][0] = columnIndicesOffset + rowIndicesOffset; //top left
        indices[i][2] = RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; //bottom left
        indices[i][1] = 1 + columnIndicesOffset + rowIndicesOffset; //top right

        indices[i + 1][0] = 1 + columnIndicesOffset + rowIndicesOffset; //top right
        indices[i + 1][2] = RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; //bottom left
        indices[i + 1][1] = 1 + RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; //bottom right

        //Shifts x position across for next chunk along grid
        columnIndicesOffset = columnIndicesOffset + 1;

        //Indexing of each chunk within row
        rowIndex++;

        //True when all chunks of the current row have been generated
        if (rowIndex == squaresPerRow)
        {
            //Resets for next row of chunks
            rowIndex = 0;
            //Resets x position for next row of chunks
            columnIndicesOffset = 0;
            //Shifts y position
            rowIndicesOffset = rowIndicesOffset + RENDER_DISTANCE;
        }
    }

    // Use indices to generate normals using 2 edges of a triangle
    for (int i = 0; i < trianglesPerTerrain; i++) // Have to -1, 32,258 triangles per terrain, OFMCPWIENWEIOPGNWEGPIOWNEGPWEOIGNWEGOEWIPEGNEWGPIOWN
    {
        // Get vertices from indices
        GLfloat* v1 = vertices[indices[i][0]];
        GLfloat* v2 = vertices[indices[i][1]];
        GLfloat* v3 = vertices[indices[i][2]];

        // Calculate edges, indices 0 = x, 1 = y, 2 = z
        vec3 edge1 = vec3(v2[0], v2[1], v2[2]) - vec3(v1[0], v1[1], v1[2]);
        vec3 edge2 = vec3(v3[0], v3[1], v3[2]) - vec3(v1[0], v1[1], v1[2]);

        // Calculate normal using cross product of edges
        vec3 normal = normalize(cross(edge2, edge1));

        // indices 6 = normal.x, 7 = normal.y, 8 = normal.z
        v1[6] = normal.x;
        v1[7] = normal.y;
        v1[8] = normal.z;
    }
}

void Terrain::SetupBuffers()
{
    //Sets index of VAO
    glGenVertexArrays(NumVAOs, VAOs);
    //Binds VAO to a buffer
    glBindVertexArray(VAOs[0]);
    //Sets indices of all required buffer objects
    glGenBuffers(NumBuffers, Buffers);

    //Binds vertex object to array buffer
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[Triangles]);
    //Allocates buffer memory for the vertices of the 'Triangles' buffer
    // Flatten 2D array
    verticesFlat = new GLfloat[MAP_SIZE * 9];
    for (int i = 0; i < MAP_SIZE; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            verticesFlat[i * 9 + j] = vertices[i][j];
        }
    }
    glBufferData(GL_ARRAY_BUFFER, MAP_SIZE * 9 * sizeof(GLfloat), verticesFlat, GL_STATIC_DRAW); // terrainVertices // Remember size and pointers... pointer only points to first element, so sizeof will only take size of first element

    //Binding & allocation for indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[Indices]);
    indicesFlat = new GLuint[trianglesPerTerrain * 3];
    for (int i = 0; i < trianglesPerTerrain; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            indicesFlat[i * 3 + j] = indices[i][j];
        }
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, trianglesPerTerrain * 3 * sizeof(GLuint), indicesFlat, GL_STATIC_DRAW);

    //Allocation & indexing of vertex attribute memory for vertex shader
    //Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Colours
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Unbind VAO
    glBindVertexArray(0);
}

void Terrain::Draw(Camera* camera, Light* light)
{
    //Drawing terrain
    glUseProgram(shaderProgram);

    //Transformations
    //Model matrix
    mat4 model = mat4(1.0f);
    //Scaling to zoom in
    model = scale(model, vec3(2.0f, 2.0f, 2.0f));
    //Elevation to look upon terrain
    model = translate(model, vec3(0.0f, -2.f, -1.5f));

    // View matrix
    mat4 view = lookAt(camera->GetPosition(), camera->GetPosition() + camera->GetFront(), camera->GetUp()); //Sets the position of the viewer, the movement direction in relation to it & the world up direction

    // Projection matrix
    mat4 projection = perspective(radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

    // Calculate normal matrix from model on CPU and send to shader via uniform
    mat3 normalMatrix = transpose(inverse(mat3(model)));

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));

    int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));

    int normalMatrixLoc = glGetUniformLocation(shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, value_ptr(normalMatrix));

    int cameraPosLoc = glGetUniformLocation(shaderProgram, "cameraPos");
    glUniformMatrix4fv(cameraPosLoc, 1, GL_FALSE, value_ptr(camera->GetPosition()));

    // Light uniforms
    int lightPosLoc = glGetUniformLocation(shaderProgram, "light.position");
    glUniformMatrix4fv(lightPosLoc, 1, GL_FALSE, value_ptr(light->GetPosition()));

    int lightAmbientLoc = glGetUniformLocation(shaderProgram, "light.ambient");
    glUniform3fv(lightAmbientLoc, 1, value_ptr(light->GetAmbient()));

    int lightDiffuseLoc = glGetUniformLocation(shaderProgram, "light.diffuse");
    glUniform3fv(lightDiffuseLoc, 1, value_ptr(light->GetDiffuse()));

    int lightSpecularLoc = glGetUniformLocation(shaderProgram, "light.specular");
    glUniform3fv(lightSpecularLoc, 1, value_ptr(light->GetSpecular()));

    // Material uniforms
    int ambientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
    glUniform3fv(ambientLoc, 1, value_ptr(ambient));

    int diffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
    glUniform3fv(diffuseLoc, 1, value_ptr(diffuse));

    int specularLoc = glGetUniformLocation(shaderProgram, "material.specular");
    glUniform3fv(specularLoc, 1, value_ptr(specular));

    int shininessLoc = glGetUniformLocation(shaderProgram, "material.shininess");
    glUniform1f(shininessLoc, shininess);

    glBindVertexArray(VAOs[0]);
    glDrawElements(GL_TRIANGLES, MAP_SIZE * 32, GL_UNSIGNED_INT, 0);

    // Unbind VAO
    glBindVertexArray(0);
}