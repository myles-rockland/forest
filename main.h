#pragma once

#include <GLFW/glfw3.h>

//Called on window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//Called on mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//Processes user input on a particular window
void ProcessUserInput(GLFWwindow* WindowIn);

GLuint terrainShader;
GLuint lightingShader;