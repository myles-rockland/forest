#include "Terrain.h"
#include "Light.h"

Light::Light(Terrain* terrain) : shaders("shaders/lighting.vert", "shaders/lighting.frag")
{
    // Set vertices (cube)
    float cubeVertices[] = {
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

    for (int i = 0; i < 3 * 6 * 6; i++)
    {
        vertices[i] = cubeVertices[i];
    }

    //position = vec3(0.0f, 1.0f, 0.0f);
    ambient = vec3(0.2f);
    diffuse = vec3(0.5f);
    specular = vec3(1.0f);

    // Get terrain vertices
    GLfloat** vertices = terrain->GetVertices();
    // Get middle vertex
    GLfloat* middleVertex = vertices[terrain->GetMapSize() / 2 - terrain->GetRenderDistance() / 2];
    // Set position to middle vertex
    origin = vec3(middleVertex[0], 0.0f, middleVertex[2]);
    position = vec3(middleVertex[0], 0.0f, middleVertex[2]);
    cout << "Light Position is { " << position.x << ", " << position.y << ", " << position.z << " }" << endl;
    // Radius needs to be based on terrain render distance...
    radius = terrain->GetVerticesOffset() * terrain->GetRenderDistance();
    //radius = terrain->GetRenderDistance();
    movementSpeed = 0.1f;

    SetupBuffers();
}

void Light::SetupBuffers()
{
    // Binding VAO for light source
    glBindVertexArray(VAOs[0]);

    // Binding VBO for light source
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Vertex attributes for light source
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VAO
    glBindVertexArray(0);
}

void Light::Update(Terrain* terrain)
{
    // Rotate light around origin at centre of terrain
    position.x = origin.x + sin(glfwGetTime() * movementSpeed) * radius;
    position.y = origin.y + cos(glfwGetTime() * movementSpeed) * radius;
}

void Light::Draw(Camera* camera)
{
    shaders.use();

    // Model matrix
    mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(0.2f));
    // View matrix
    mat4 view = lookAt(camera->GetPosition(), camera->GetPosition() + camera->GetFront(), camera->GetUp()); //Sets the position of the viewer, the movement direction in relation to it & the world up direction
    //Projection matrix
    mat4 projection = perspective(radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

    // Set uniforms
    shaders.setMat4("model", model);

    shaders.setMat4("view", view);

    shaders.setMat4("projection", projection);

    glBindVertexArray(VAOs[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Unbind VAO
    glBindVertexArray(0);
}

vec3 Light::GetPosition() const { return position; }
vec3 Light::GetAmbient() const { return ambient; }
vec3 Light::GetDiffuse() const { return diffuse; }
vec3 Light::GetSpecular() const { return specular; }