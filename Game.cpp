#include "Game.h"
using namespace std;

Game* Game::instance = nullptr;

Game::Game() : deltaTime(0.0f), lastFrame(0.0f)
{
    instance = this;

    camera = new Camera();

    // Error handling for initialisation of GLFW
    if (InitialiseGLFW() == -1)
    {
        isRunning = false;
    }
    else
    {
        isRunning = true;
    }

    //Initialisation of GLEW
    glewInit();
}

Game::~Game()
{
    delete instance;
    delete camera;
    delete window;
}

void Game::Run()
{
    // Create Terrain
    terrain = new Terrain();

    // Create Player
    player = new Player(terrain, camera);

    // Create Light
    light = new Light();

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    //Render loop
    while (glfwWindowShouldClose(window) == false && isRunning)
    {
        //Time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Input
        ProcessUserInput(window); //Takes user input

        //Rendering
        glClearColor(0.1f, 0.1f, 0.3f, 1.0f); //Colour to display on cleared window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clears the colour buffer

        // Drawing light source
        light->Draw(player->GetCamera());

        // Drawing terrain
        terrain->Draw(player->GetCamera(), light);

        // Refreshing
        glfwSwapBuffers(window); // Swaps the colour buffer
        glfwPollEvents(); // Polls for all GLFW events
    }

    //Safely terminates GLFW
    glfwTerminate();
}

void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //Resizes window based on contemporary width & height values
    glViewport(0, 0, width, height);
}

void Game::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (instance && instance->camera)
    {
        instance->camera->MouseCallback(window, xpos, ypos);
    }
}

void Game::ProcessUserInput(GLFWwindow* windowIn)
{
    //Closes window on 'exit' key press
    if (glfwGetKey(windowIn, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(windowIn, true);
    }

    if (player)
    {
        player->ProcessInput(windowIn, deltaTime);
    }
}

int Game::InitialiseGLFW()
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