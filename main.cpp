#include "main.h"

using namespace std;
using namespace glm;

//VAO vertex attribute positions in correspondence to vertex attribute type
enum VAO_IDs { Triangles, Indices, Colours, Textures, NumVAOs = 2 };
//VAOs
GLuint VAOs[NumVAOs];

//Buffer types
enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 };
//Buffer objects
GLuint Buffers[NumBuffers];

//Transformations
//Relative position within world space
vec3 cameraPosition = vec3(0.0f, 0.0f, 3.0f);
//The direction of travel
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
//Up position within world space
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

//Camera sidways rotation
float cameraYaw = -90.0f;
//Camera vertical rotation
float cameraPitch = 0.0f;
//Determines if first entry of mouse into window
bool mouseFirstEntry = true;
//Positions of camera from given last frame
float cameraLastXPos = (float)WINDOW_WIDTH / 2.0f;
float cameraLastYPos = (float)WINDOW_HEIGHT / 2.0f;

//Time
//Time change
float deltaTime = 0.0f;
//Last value of time change
float lastFrame = 0.0f;

#define RENDER_DISTANCE 128 //Render width of map
#define MAP_SIZE RENDER_DISTANCE * RENDER_DISTANCE //Size of map in x & z space

//Amount of chunks across one dimension
const int squaresRow = RENDER_DISTANCE - 1;
//Two triangles per square to form a 1x1 chunk
const int trianglesPerSquare = 2;
//Amount of triangles on map
const int trianglesGrid = squaresRow * squaresRow * trianglesPerSquare;

int main()
{
    if (InitialiseGLFW() == -1)
    {
        return -1;
    }

    //Initialisation of GLEW
    glewInit();

    //Load terrain shaders
    ShaderInfo terrainShaders[] =
    {
        { GL_VERTEX_SHADER, "shaders/terrain.vert" },
        { GL_FRAGMENT_SHADER, "shaders/terrain.frag" },
        { GL_NONE, NULL }
    };

    terrainShader = LoadShaders(terrainShaders);

    //Load lighting shaders
    ShaderInfo lightingShaders[] =
    {
        { GL_VERTEX_SHADER, "shaders/lighting.vert" },
        { GL_FRAGMENT_SHADER, "shaders/lighting.frag" },
        { GL_NONE, NULL }
    };

    lightingShader = LoadShaders(lightingShaders);

    // Cube light source vertices
    float lightSourceVertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
    };

    //Generation of height map vertices

    GLfloat** terrainVertices = new GLfloat*[MAP_SIZE];
    for (int i = 0; i < MAP_SIZE; i++)
    {
        terrainVertices[i] = new GLfloat[9]; // 3 position components, 3 colour components, 3 normal components
    }

    //Positions to start drawing from
    float drawingStartPosition = 1.0f;
    float columnVerticesOffset = drawingStartPosition;
    float rowVerticesOffset = drawingStartPosition;

    int rowIndex = 0;
    for (int i = 0; i < MAP_SIZE; i++)
    {
        //Generation of x & z vertices for horizontal plane
        terrainVertices[i][0] = columnVerticesOffset;
        terrainVertices[i][2] = rowVerticesOffset;

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
    int terrainSeed = rand() % 100;
    //Sets seed for noise
    TerrainNoise.SetSeed(terrainSeed);

    //Biome noise
    FastNoiseLite BiomeNoise;
    BiomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    BiomeNoise.SetFrequency(0.05f);
    int biomeSeed = rand() % 100;
    TerrainNoise.SetSeed(biomeSeed);

    //Terrain vertex index
    int i = 0;
    //Using x & y nested for loop in order to apply noise 2-dimensionally
    for (int y = 0; y < RENDER_DISTANCE; y++)
    {
        for (int x = 0; x < RENDER_DISTANCE; x++)
        {
            //Setting of height from 2D noise value at respective x & y coordinate
            terrainVertices[i][1] = TerrainNoise.GetNoise((float)x, (float)y);

            //Retrieval of biome to set
            float biomeValue = BiomeNoise.GetNoise((float)x, (float)y);

            if (biomeValue <= -0.75f) //Plains
            {
                terrainVertices[i][3] = 0.0f;
                terrainVertices[i][4] = 0.75f;
                terrainVertices[i][5] = 0.25f;
            }
            else //Desert
            {
                terrainVertices[i][3] = 1.0f;
                terrainVertices[i][4] = 1.0f;
                terrainVertices[i][5] = 0.5f;
            }

            i++;
        }
    }

    //Generation of height map indices
    GLuint** terrainIndices = new GLuint * [trianglesGrid];
    for (int i = 0; i < trianglesGrid; i++)
    {
        terrainIndices[i] = new GLuint[3];
    }

    //Positions to start mapping indices from
    int columnIndicesOffset = 0;
    int rowIndicesOffset = 0;

    //Generation of map indices in the form of chunks (1x1 right angle triangle squares)
    rowIndex = 0;
    for (int i = 0; i < trianglesGrid - 1; i += 2)
    {
        terrainIndices[i][0] = columnIndicesOffset + rowIndicesOffset; //top left
        terrainIndices[i][2] = RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; //bottom left
        terrainIndices[i][1] = 1 + columnIndicesOffset + rowIndicesOffset; //top right

        terrainIndices[i + 1][0] = 1 + columnIndicesOffset + rowIndicesOffset; //top right
        terrainIndices[i + 1][2] = RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; //bottom left
        terrainIndices[i + 1][1] = 1 + RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; //bottom right

        //Shifts x position across for next chunk along grid
        columnIndicesOffset = columnIndicesOffset + 1;

        //Indexing of each chunk within row
        rowIndex++;

        //True when all chunks of the current row have been generated
        if (rowIndex == squaresRow)
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
    for (int i = 0; i < trianglesGrid; i++)
    {
        // Get vertices from indices
        GLfloat* v1 = terrainVertices[terrainIndices[i][0]];
        GLfloat* v2 = terrainVertices[terrainIndices[i][1]];
        GLfloat* v3 = terrainVertices[terrainIndices[i][2]];

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
    GLfloat* tVerticesFlat = new GLfloat[MAP_SIZE * 9];
    for (int i = 0; i < MAP_SIZE; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            tVerticesFlat[i * 9 + j] = terrainVertices[i][j];
        }
    }
    glBufferData(GL_ARRAY_BUFFER, MAP_SIZE * 9 * sizeof(GLfloat), tVerticesFlat, GL_STATIC_DRAW); // terrainVertices // Remember size and pointers... pointer only points to first element, so sizeof will only take size of first element

    //Binding & allocation for indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[Indices]);
    GLuint* tIndicesFlat = new GLuint[trianglesGrid * 3];
    for (int i = 0; i < trianglesGrid; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            tIndicesFlat[i * 3 + j] = terrainIndices[i][j];
        }
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, trianglesGrid * 3 * sizeof(GLuint), tIndicesFlat, GL_STATIC_DRAW);

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

    // Binding VAO for light source
    glBindVertexArray(VAOs[1]);

    // Binding VBO for light source
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightSourceVertices), lightSourceVertices, GL_STATIC_DRAW);
    // Vertex attributes for light source
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Unbinding
    //glBindVertexArray(0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //Projection matrix
    mat4 projection = perspective(radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    //Render loop
    while (glfwWindowShouldClose(window) == false)
    {
        //Time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Input
        ProcessUserInput(window); //Takes user input

        //Rendering
        glClearColor(0.25f, 0.0f, 1.0f, 1.0f); //Colour to display on cleared window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clears the colour buffer

        // Drawing light source
        glUseProgram(lightingShader);
        vec3 lightPos(0.0f, 1.0f, 0.0f);
        mat4 model = glm::mat4(1.0f);
        mat4 view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp); //Sets the position of the viewer, the movement direction in relation to it & the world up direction
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        int modelLoc = glGetUniformLocation(lightingShader, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

        int viewLoc = glGetUniformLocation(lightingShader, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));

        int projectionLoc = glGetUniformLocation(lightingShader, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));

        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Drawing terrain
        glUseProgram(terrainShader);

        //Transformations
        //Model matrix
        model = mat4(1.0f);
        //Scaling to zoom in
        model = scale(model, vec3(2.0f, 2.0f, 2.0f));
        //Looking straight forward
        model = rotate(model, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
        //Elevation to look upon terrain
        model = translate(model, vec3(0.0f, -2.f, -1.5f));

        // Calculate normal matrix from model on CPU and send to shader via uniform
        mat3 normalMatrix = transpose(inverse(mat3(model)));

        // Terrain material vectors
        vec3 materialAmbient(1.0f);
        vec3 materialDiffuse(1.0f);
        vec3 materialSpecular(0.1f);
        GLfloat materialShininess = 2.0f;

        // Light source material vectors
        vec3 lightAmbient(0.2f);
        vec3 lightDiffuse(0.5f);
        vec3 lightSpecular(1.0f);

        modelLoc = glGetUniformLocation(terrainShader, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
        
        viewLoc = glGetUniformLocation(terrainShader, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));

        projectionLoc = glGetUniformLocation(terrainShader, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));

        int normalMatrixLoc = glGetUniformLocation(terrainShader, "normalMatrix");
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, value_ptr(normalMatrix));

        int cameraPosLoc = glGetUniformLocation(terrainShader, "cameraPos");
        glUniformMatrix4fv(cameraPosLoc, 1, GL_FALSE, value_ptr(cameraPosition));

        // Light uniforms
        int lightPosLoc = glGetUniformLocation(terrainShader, "light.position");
        glUniformMatrix4fv(lightPosLoc, 1, GL_FALSE, value_ptr(lightPos));

        int lightAmbientLoc = glGetUniformLocation(terrainShader, "light.ambient");
        glUniform3fv(lightAmbientLoc, 1, value_ptr(lightAmbient));

        int lightDiffuseLoc = glGetUniformLocation(terrainShader, "light.diffuse");
        glUniform3fv(lightDiffuseLoc, 1, value_ptr(lightDiffuse));

        int lightSpecularLoc = glGetUniformLocation(terrainShader, "light.specular");
        glUniform3fv(lightSpecularLoc, 1, value_ptr(lightSpecular));

        // Material uniforms
        int ambientLoc = glGetUniformLocation(terrainShader, "material.ambient");
        glUniform3fv(ambientLoc, 1, value_ptr(materialAmbient));

        int diffuseLoc = glGetUniformLocation(terrainShader, "material.diffuse");
        glUniform3fv(diffuseLoc, 1, value_ptr(materialDiffuse));

        int specularLoc = glGetUniformLocation(terrainShader, "material.specular");
        glUniform3fv(specularLoc, 1, value_ptr(materialSpecular));

        int shininessLoc = glGetUniformLocation(terrainShader, "material.shininess");
        glUniform1f(shininessLoc, materialShininess);

        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, MAP_SIZE * 32, GL_UNSIGNED_INT, 0);

        //Refreshing
        glfwSwapBuffers(window); //Swaps the colour buffer
        glfwPollEvents(); //Queries all GLFW events
    }

    //Safely terminates GLFW
    glfwTerminate();

    // Delete the arrays
    for (int i = 0; i < MAP_SIZE; i++) {
        delete[] terrainVertices[i]; // Free each row
    }
    delete[] terrainVertices;
    delete[] tVerticesFlat;
    for (int i = 0; i < MAP_SIZE; i++) {
        delete[] terrainIndices[i]; // Free each row
    }
    delete[] terrainIndices;
    delete[] tIndicesFlat;

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //Resizes window based on contemporary width & height values
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    //Initially no last positions, so sets last positions to current positions
    if (mouseFirstEntry)
    {
        cameraLastXPos = (float)xpos;
        cameraLastYPos = (float)ypos;
        mouseFirstEntry = false;
    }

    //Sets values for change in position since last frame to current frame
    float xOffset = (float)xpos - cameraLastXPos;
    float yOffset = cameraLastYPos - (float)ypos;

    //Sets last positions to current positions for next frame
    cameraLastXPos = (float)xpos;
    cameraLastYPos = (float)ypos;

    //Moderates the change in position based on sensitivity value
    const float sensitivity = 0.025f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    //Adjusts yaw & pitch values against changes in positions
    cameraYaw += xOffset;
    cameraPitch += yOffset;

    //Prevents turning up & down beyond 90 degrees to look backwards
    if (cameraPitch > 89.0f)
    {
        cameraPitch = 89.0f;
    }
    else if (cameraPitch < -89.0f)
    {
        cameraPitch = -89.0f;
    }

    //Modification of direction vector based on mouse turning
    vec3 direction;
    direction.x = cos(radians(cameraYaw)) * cos(radians(cameraPitch));
    direction.y = sin(radians(cameraPitch));
    direction.z = sin(radians(cameraYaw)) * cos(radians(cameraPitch));
    cameraFront = normalize(direction);
}

void ProcessUserInput(GLFWwindow* WindowIn)
{
    //Closes window on 'exit' key press
    if (glfwGetKey(WindowIn, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(WindowIn, true);
    }

    //Extent to which to move in one instance
    const float movementSpeed = 1.0f * deltaTime;
    //WASD controls
    if (glfwGetKey(WindowIn, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPosition += movementSpeed * cameraFront;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPosition -= movementSpeed * cameraFront;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPosition -= normalize(cross(cameraFront, cameraUp)) * movementSpeed;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPosition += normalize(cross(cameraFront, cameraUp)) * movementSpeed;
    }
}

int InitialiseGLFW()
{
    //Initialisation of GLFW
    glfwInit();
    //Initialisation of 'GLFWwindow' object
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Forest Game", NULL, NULL);

    //Checks if window has been successfully instantiated
    if (window == NULL)
    {
        cout << "GLFW Window did not instantiate\n";
        glfwTerminate();
        return -1;
    }

    //Sets cursor to automatically bind to window & hides cursor pointer
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Binds OpenGL to window
    glfwMakeContextCurrent(window);

    //Sets the viewport size within the window to match the window size of 1280x720
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    //Sets the framebuffer_size_callback() function as the callback for the window resizing event
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Sets the mouse_callback() function as the callback for the mouse movement event
    glfwSetCursorPosCallback(window, mouse_callback);

    return 0;
}