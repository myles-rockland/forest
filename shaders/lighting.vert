#version 460
//Triangle position with values retrieved from main.cpp
layout (location = 0) in vec3 position;

//Model-View-Projection Matrix
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //Transformation applied to vertices
    gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);
}