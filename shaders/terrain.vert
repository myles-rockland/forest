#version 460
//Triangle position with values retrieved from main.cpp
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 colourVertex;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;
layout (location = 4) in float aMixFactor;

//Colour to send
out vec3 Ambient;
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;
out float MixFactor;

// Material
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;
//Model-View-Projection Matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
    //Transformation applied to vertices
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalMatrix * aNormal;

    //Sending texture coordinates to next stage
    Ambient = colourVertex * material.ambient;

    TexCoord = aTexCoord;
    MixFactor = aMixFactor;
}