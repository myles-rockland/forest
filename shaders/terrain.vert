#version 460
//Triangle position with values retrieved from main.cpp
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 colourVertex;
layout (location = 2) in vec3 aNormal;

//Colour to send
out vec3 Ambient;
out vec3 Normal;
out vec3 FragPos;

//Model-View-Projection Matrix
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;
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
}