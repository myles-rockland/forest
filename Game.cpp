#include "Game.h"
using namespace std;

Game* Game::instance = nullptr;

Game::Game() : deltaTime(0.0f), lastFrame(0.0f), isRunning(true), camera(nullptr), terrain(nullptr), player(nullptr), light(nullptr), monster(nullptr), signature(nullptr)
{
    instance = this;

    camera = new Camera();

    // Initialise irrKlang system
    soundEngine = irrklang::createIrrKlangDevice();
    // Error handling to check if irrKlang has been susccessfully initialised
    if (!soundEngine)
    {
        isRunning = false;
        return;
    }

    //Initialisation of GLFW
    glfwInit();

    //Initialisation of 'GLFWwindow' object
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Forest Game", NULL, NULL);

    // Error handling to check if window has been successfully instantiated
    if (window == NULL)
    {
        cout << "GLFW Window did not instantiate\n";
        glfwTerminate();
        isRunning = false;
        return;
    }

    //Sets cursor to automatically bind to window & hides cursor pointer
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Binds OpenGL to window
    glfwMakeContextCurrent(window);

    // Initialisation of GLAD and error handling if it fails
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "GLAD failed to initialise" << endl;
        isRunning = false;
        return;
    }

    //Sets the viewport size within the window to match the window size of 1280x720
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    //Sets the framebuffer_size_callback() function as the callback for the window resizing event
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Sets the mouse_callback() function as the callback for the mouse movement event
    glfwSetCursorPosCallback(window, mouse_callback);

    srand(time(0));
}

Game::~Game()
{
    delete instance;
    delete camera;
    delete terrain;
    delete player;
    delete light;
    delete monster;
    delete[] collectables;
    delete[] trees;
    soundEngine->drop(); // Delete sound engine
}

void Game::Run()
{
    // Create Terrain
    terrain = new Terrain();
    if (!terrain->IsTexturesLoaded())
    {
        isRunning = false;
    }

    // Create Player
    player = new Player(terrain, camera);

    // Create Monster
    monster = new Monster(terrain);

    // Create Collectables
    for (int i = 0; i < 5; i++)
    {
        collectables[i] = new Collectable(terrain, player);
    }

    // Create Trees
    for (int i = 0; i < 10; i++)
    {
        trees[i] = new Tree(terrain);
    }

    // Create Light
    light = new Light(terrain);

    // Create Signature
    signature = new Signature();
    if (!signature->IsTexturesLoaded())
    {
        isRunning = false;
    }

    // Play background music looped
    soundEngine->play2D("./audio/ghostly_presence.ogg", true);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    //glEnable(GL_CULL_FACE);

    // Enable blending for transparency in textures
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Input/Update/Render Game loop
    while (glfwWindowShouldClose(window) == false && isRunning)
    {
        //Time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        ProcessUserInput(window); //Takes user input

        // Updating objects
        // Update monster
        // Move monster towards player
        monster->Update(player->GetCamera(), deltaTime);
        // Check if monster caught player
        if (monster->GetCaughtPlayer())
        {
            isRunning = false;
        }

        // Update light
        light->Update(terrain);

        // Checking if object collected
        for (int i = 0; i < 5; i++)
        {
            Collectable* collectable = collectables[i];
            collectable->Update();
        }

        //Rendering
        vec3 clearColour = light->GetClearColour();
        glClearColor(clearColour.r, clearColour.g, clearColour.b, 1.0f); //Colour to display on cleared window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clears the colour buffer

        // Drawing light source
        light->Draw(player->GetCamera());

        // Drawing terrain
        terrain->Draw(player->GetCamera(), light);

        // Drawing collectables
        for (int i = 0; i < 5; i++)
        {
            Collectable* collectable = collectables[i];
            if (!collectable->IsCollected())
            {
                collectable->Draw(player->GetCamera());
            }
        }

        // Drawing monster
        monster->Draw(player->GetCamera(), light);

        // Draw trees
        for (int i = 0; i < 10; i++)
        {
            trees[i]->Draw(player->GetCamera());
        }

        // Drawing signature
        signature->Draw(player->GetCamera());

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