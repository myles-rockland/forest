#include "Signature.h"

Signature::Signature() : shaders("shaders/signature.vert", "shaders/signature.frag")
{
    float vertices[] = {
        // Positions            // Textures
        -0.9f, 1.0f, 0.0f,       1.0f, 1.0f, // top-right
        -0.9f, 0.9f, 0.0f,      1.0f, 0.0f, // bottom-right
        -1.0f, 0.9f, 0.0f,     0.0f, 0.0f, // bottom-left
        -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // top-left
    };

    unsigned int indices[] = {
        0, 1, 3, // First triangle
        1, 2, 3 // Second triangle
    };

    for (int i = 0; i < 20; i++)
    {
        this->vertices[i] = vertices[i];
    }
    for (int i = 0; i < 6; i++)
    {
        this->indices[i] = indices[i];
    }

    position = vec3(0.0f);

    SetupBuffers();
    texturesLoaded = true;
    SetupTextures();
}

void Signature::SetupBuffers()
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // terrainVertices // Remember size and pointers... pointer only points to first element, so sizeof will only take size of first element

    //Binding & allocation for indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[Indices]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Allocation & indexing of vertex attribute memory for vertex shader
    // Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Textures
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);
    // Unbind Textures
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Signature::SetupTextures()
{
    //Textures to generate
    glGenTextures(NumBuffers, Buffers);

    //Binding texture to type 2D texture
    glBindTexture(GL_TEXTURE_2D, Buffers[Textures]);

    //Selects x axis (S) of texture bound to GL_TEXTURE_2D & sets to repeat beyond normalised coordinates
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //Selects y axis (T) equivalently
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //Parameters that will be sent & set based on retrieved texture
    int width, height, colourChannels;

    // Flip loaded textures on y axis
    stbi_set_flip_vertically_on_load(true);

    //Retrieves texture data
    unsigned char* data = stbi_load("media/signature.png", &width, &height, &colourChannels, 0);

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
}

void Signature::Draw()
{
    shaders.use();

    glBindTexture(GL_TEXTURE_2D, Buffers[Textures]);
    glBindVertexArray(VAOs[0]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Signature::IsTexturesLoaded() const { return texturesLoaded; }