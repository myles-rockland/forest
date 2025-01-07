#include "Terrain.h"

using namespace glm;

Terrain::Terrain() : vertices(nullptr), indices(nullptr), RENDER_DISTANCE(128), MAP_SIZE(RENDER_DISTANCE * RENDER_DISTANCE), trianglesPerSquare(2), squaresPerRow(RENDER_DISTANCE-1), trianglesPerTerrain(squaresPerRow * squaresPerRow * trianglesPerSquare), VERTICES_OFFSET(0.0625f), shaders("shaders/terrain.vert", "shaders/terrain.frag"), GRASS_BIOME_VALUE(-0.75f), MIX_BIOME_VALUE(-0.6f) // VERTICES_OFFSET(0.0625f)
{

    GenerateVertices();
    GenerateIndices();

    ambient = vec3(1.0f);
    diffuse = vec3(1.0f);
    specular = vec3(0.1f);
    shininess = 2.0f;

    SetupBuffers();
    texturesLoaded = true;
    SetupTextures();
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
        vertices[i] = new GLfloat[12]; // 3 position components, 3 colour components, 3 normal components, 2 texture coordinate components, 1 texture mix factor
        vertices[i][0] = 0.0f; // x position
        vertices[i][1] = 0.0f; // y position
        vertices[i][2] = 0.0f; // z position
        vertices[i][3] = 0.0f; // r
        vertices[i][4] = 0.0f; // g
        vertices[i][5] = 0.0f; // b
        vertices[i][6] = 0.0f; // x normal
        vertices[i][7] = 0.0f; // y normal
        vertices[i][8] = 0.0f; // z normal
        vertices[i][9] = 0.0f; // u
        vertices[i][10] = 0.0f; // v
        vertices[i][11] = 0.0f; // texture mix factor
    }

    //Positions to start drawing from
    float drawingStartPosition = 0.0f;
    float columnVerticesOffset = drawingStartPosition;
    float rowVerticesOffset = drawingStartPosition;

    int rowIndex = 0;
    for (int i = 0; i < MAP_SIZE; i++)
    {
        //Generation of x & z vertices for horizontal plane
        vertices[i][0] = columnVerticesOffset;
        vertices[i][2] = rowVerticesOffset;

        //Shifts x position across for next triangle along grid
        columnVerticesOffset += VERTICES_OFFSET;

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
            rowVerticesOffset += VERTICES_OFFSET;
        }
    }

    //Setting noise type to Perlin
    HeightMapNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    //Sets the noise scale
    HeightMapNoise.SetFrequency(0.05f);
    //Generates a random seed between integers 0 & 100
    int terrainSeed = rand() % 100;
    //Sets seed for noise
    HeightMapNoise.SetSeed(terrainSeed);

    //Biome noise
    BiomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    BiomeNoise.SetFrequency(0.05f);
    int biomeSeed = rand() % 100;
    BiomeNoise.SetSeed(biomeSeed);

    //Using x & y nested for loop in order to apply noise 2-dimensionally, as well as generate texture coordinates
    for (int y = 0; y < RENDER_DISTANCE; y++)
    {
        for (int x = 0; x < RENDER_DISTANCE; x++)
        {
            int index = y * RENDER_DISTANCE + x;
            if (index < MAP_SIZE)
            {
                //Setting of height from 2D noise value at respective x & y coordinate
                vertices[index][1] = HeightMapNoise.GetNoise((float)x, (float)y);

                //Retrieval of biome to set
                float biomeValue = BiomeNoise.GetNoise((float)x, (float)y);

                if (biomeValue <= GRASS_BIOME_VALUE) // Grass biome
                {
                    vertices[index][3] = 0.0f;
                    vertices[index][4] = 0.75f;
                    vertices[index][5] = 0.25f;

                    vertices[index][11] = 0.0f; // 0% mix, so 100% texture 1, 0% texture 2
                }
                else if (biomeValue <= MIX_BIOME_VALUE) // Mix biome
                {
                    vertices[index][3] = 0.5f;
                    vertices[index][4] = 0.875f;
                    vertices[index][5] = 0.375f;

                    vertices[index][11] = 0.5f; // 50% mix, so 50% texture 1, 50% texture 2
                }
                else // Dirt path biome (to look like path where people have walked)
                {
                    vertices[index][3] = 1.0f;
                    vertices[index][4] = 1.0f;
                    vertices[index][5] = 0.5f;

                    vertices[index][11] = 1.0f; // 100% mix, so 0% texture 1, 100% texture 2
                }

                // Setting texture coordinates
                vertices[index][9] = (float)x;
                vertices[index][10] = (float)y;
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
        indices[i][1] = RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; //bottom left
        indices[i][2] = 1 + columnIndicesOffset + rowIndicesOffset; //top right

        indices[i + 1][0] = 1 + columnIndicesOffset + rowIndicesOffset; //top right
        indices[i + 1][1] = RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; //bottom left
        indices[i + 1][2] = 1 + RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; //bottom right

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
    verticesFlat = new GLfloat[MAP_SIZE * 12];
    for (int i = 0; i < MAP_SIZE; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            verticesFlat[i * 12 + j] = vertices[i][j];
        }
    }
    glBufferData(GL_ARRAY_BUFFER, MAP_SIZE * 12 * sizeof(GLfloat), verticesFlat, GL_STATIC_DRAW); // terrainVertices // Remember size and pointers... pointer only points to first element, so sizeof will only take size of first element

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Colours
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //Textures
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);

    //Texture mix factor
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);

    // Unbind VAO
    glBindVertexArray(0);
}

void Terrain::SetupTextures()
{
    // TEXTURE 1
    //Textures to generate
    glGenTextures(NumBuffers, Buffers);

    //Binding texture to type 2D texture
    glBindTexture(GL_TEXTURE_2D, Buffers[Textures1]);

    //Selects x axis (S) of texture bound to GL_TEXTURE_2D & sets to repeat beyond normalised coordinates
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //Selects y axis (T) equivalently
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Parameters that will be sent & set based on retrieved texture
    int width, height, colourChannels;

    //Retrieves texture data
    unsigned char* data = stbi_load("media/grass.png", &width, &height, &colourChannels, 0);

    if (data) //If retrieval successful
    {
        //Generation of texture from retrieved texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // Might need GL_RGBA
        //Automatically generates all required mipmaps on bound texture
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else //If retrieval unsuccessful
    {
        cout << "Failed to load texture.\n";
        texturesLoaded = false;
    }

    //Clears retrieved texture from memory
    stbi_image_free(data);

    // TEXTURE 2

    //Binding texture to type 2D texture
    glBindTexture(GL_TEXTURE_2D, Buffers[Textures2]);

    //Selects x axis (S) of texture bound to GL_TEXTURE_2D & sets to repeat beyond normalised coordinates
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //Selects y axis (T) equivalently
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Retrieves texture data
    data = stbi_load("media/dirty-grass.jpg", &width, &height, &colourChannels, 0);

    if (data) //If retrieval successful
    {
        //Generation of texture from retrieved texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        //Automatically generates all required mipmaps on bound texture
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else //If retrieval unsuccessful
    {
        cout << "Failed to load texture.\n";
        texturesLoaded = false;
    }

    //Clears retrieved texture from memory
    stbi_image_free(data);
}

void Terrain::Draw(Camera* camera, Light* light)
{
    // Bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Buffers[Textures1]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Buffers[Textures2]);

    //Drawing terrain
    shaders.use();

    //Transformations
    //Model matrix
    mat4 model = mat4(1.0f);
    //Scaling to zoom in
    //model = scale(model, vec3(2.0f, 2.0f, 2.0f));
    //Elevation to look upon terrain
    //model = translate(model, vec3(0.0f, -2.0f, -1.5f));

    // View matrix
    mat4 view = lookAt(camera->GetPosition(), camera->GetPosition() + camera->GetFront(), camera->GetUp()); //Sets the position of the viewer, the movement direction in relation to it & the world up direction

    // Projection matrix
    mat4 projection = perspective(radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

    // Calculate normal matrix from model on CPU and send to shader via uniform
    mat3 normalMatrix = transpose(inverse(mat3(model)));

    // MVP uniforms
    shaders.setMat4("model", model);

    shaders.setMat4("view", view);

    shaders.setMat4("projection", projection);

    shaders.setMat3("normalMatrix", normalMatrix);

    shaders.setVec3("cameraPos", camera->GetPosition());

    // Light uniforms
    shaders.setVec3("light.position", light->GetPosition());

    shaders.setVec3("light.ambient", light->GetAmbient());

    shaders.setVec3("light.diffuse", light->GetDiffuse());

    shaders.setVec3("light.specular", light->GetSpecular());

    // Material uniforms
    shaders.setVec3("material.ambient", ambient);

    shaders.setVec3("material.diffuse", diffuse);

    shaders.setVec3("material.specular", specular);

    shaders.setFloat("material.shininess", shininess);

    // Texture units
    shaders.setInt("texture1", 0);
    shaders.setInt("texture2", 1);

    // Bind vertex array
    glBindVertexArray(VAOs[0]);
    glDrawElements(GL_TRIANGLES, MAP_SIZE * 32, GL_UNSIGNED_INT, 0);

    // Unbind VAO
    glBindVertexArray(0);
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
    // Reset active texture unit to 0
    glActiveTexture(GL_TEXTURE0);
    
}

FastNoiseLite Terrain::GetHeightMapNoise() const { return HeightMapNoise; }
FastNoiseLite Terrain::GetBiomeNoise() const { return BiomeNoise; }
int Terrain::GetRenderDistance() const { return RENDER_DISTANCE; }
int Terrain::GetMapSize() const { return MAP_SIZE; }
GLfloat** Terrain::GetVertices() const { return vertices; }
float Terrain::GetVerticesOffset() const { return VERTICES_OFFSET; }
bool Terrain::IsTexturesLoaded() const { return texturesLoaded; }
float Terrain::GetGrassBiomeValue() const { return GRASS_BIOME_VALUE; }
float Terrain::GetMixBiomeValue() const { return MIX_BIOME_VALUE; }