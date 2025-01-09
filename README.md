# Forest Game Report
Forest Game is a horror game about collecting pages.

## Dependencies
- Assimp
- GLAD
- GLFW
- GLM
- LearnOpenGL files, including mesh.h, model.h, shader.h, shader_m.h
- stbi_image.h header file
- irrKlang

## Third-party Assets
- Monster model and textures - https://www.mixamo.com/#/?page=1&query=parasite&type=Character
- Grass texture - https://opengameart.org/node/21686
- Dirty Grass texture - https://opengameart.org/content/4k-seamless-grass-dirt-ground-texture-with-all-shader-maps-dirtygrassseamlesscolorjpg
- Tree model and textures - https://sketchfab.com/3d-models/pine-tree-e52769d653cd4e52a4acff3041961e65#download
- Ghostly Presence music (Website: Bensound, Artist: FoePound, License code: ZCM0KIEXOOQO8DYV) - https://www.bensound.com/royalty-free-music/track/ghostly-presence-creepy-dark
- Paper Grab sfx - https://pixabay.com/sound-effects/paper-grab-259214/
- Walking on Leaves sfx - https://pixabay.com/sound-effects/walking-on-leaves-260279/
- Zombie Breathing sfx - https://pixabay.com/sound-effects/zombie-breathing-70682/

## YouTube Video
YouTube Video: [insert some youtube video]

## Game Objective
The objective of Forest Game is to search the environment for five pages while keeping an eye out for the monster.

Walk into a page to collect it.

The monster is always chasing you. To stop it, look at it.

If you collect all five pages, you win the game.

If the monster catches you, you lose.

## Controls
- **WASD Keys** - Move forwards, left, right and backwards
- **Escape** - Quit game

## Design Diagrams

### Class Diagram
The following diagram highlights the main classes, member variables and methods within the game:
![Class Diagram](./images/forest_class_diagram.png)

## Programming Patterns

### Game Loop
![Game loop pattern](./images/game_loop.png)
### Update Method
![Update Method pattern in Monster](./images/monster_update_method.png)
![Update Method pattern in Light](./images/light_update_method.png)
![Update Method pattern in Collectable](./images/collectable_update_method.png)
### Composition
![Composition pattern in Game](./images/composition.png)

## Game Features/Mechanics + Example Code

### Moving Player
The player can move and look around the terrain, restricting the player to stay within the boundaries. The player's y position is based on the terrain's heightmap.
![Player movement](./images/player_movement.gif)
![Code example of player movement](./images/player_movement.png)
![Code example of player movement, restricted to the terrain, and updating y position based on terrain heightmap](./images/player_movement2.png)
### Moving Monster
The monster will move and rotate towards the player while they are not looking. The monster's y position is also based on the terrain's heightmap.
![Monster movement](./images/monster_movement.gif)
![Code example of monster movement](./images/monster_movement_code.png)
### Page Collection
Pages will disappear when the player walks into them and collects them.
![Page collection](./images/page_collection.gif)
![Code example of collecting a page](./images/page_collection.png)
![Code example of collecting a page, disabling drawing if it is collected](./images/page_collection2.png)
### Model-View-Projection
Everything in the game is rendered using Model-View-Projection matrices.
![C++ code example of Model-View-Projection matrices in use for terrain](./images/mvp_cpp_terrain.png)
![GLSL vertex shader code example of Model-View-Projection matrices in use for terrain](./images/mvp_vert_terrain.png)
![C++ code example of Model-View-Projection matrices in use for monster](./images/mvp_cpp_monster1.png)
![C++ code example of Model-View-Projection matrices in use for monster](./images/mvp_cpp_monster2.png)
![GLSL vertex shader code example of Model-View-Projection matrices in use for monster](./images/mvp_vert_monster.png)
### Procedural Terrain Generation
The terrain is generated procedurally with three different biomes, giving a different terrain every time you play the game.
![Procedural terrain generation](./images/pcg1.png)
![Procedural terrain generation](./images/pcg2.png)
![Code example of seeding of random function based on current time](./images/pcg_code1.png)
![Code example of terrain generation of vertices](./images/pcg_code2.png)
![Code example of terrain generation of vertices x and z position](./images/pcg_code2-5.png)
![Code example of terrain generation of vertices y position based on heightmap](./images/pcg_code3.png)
![Code example of terrain generation of vertices biomes based on cellular noise](./images/pcg_code4.png)
### Texture mixing
Textures in the terrain are mixed together to create "middle" biomes.
![Texture mixing](./images/grass.png)
![Texture mixing](./images/dirty-grass.jpg)
![Texture mixing](./images/texture_mixing.png)
![Code example of texture mixing setup in C++](./images/texture_mixing_code1.png)
![Code example of texture mixing in fragment shader](./images/texture_mixing_code2.png)
### Scene Animations
Some objects in the scene are animated, moving/rotating over time.
![Scene animations with moving sun](./images/sunset.gif)
![Code example of sunset](./images/sunset_code.png)
![Code example of sunset](./images/sunset_code2.png)
![Scene animations with floating and rotating collectables](./images/page_animation.gif)
![Code example of floating and rotating collectables](./images/page_animation_code.png)
### Loaded Models with Textures
Models are loaded with textures using the Model class. Multiple formats are used.
![Loaded model with textures](./images/model_monster.png)
![Loaded model with textures](./images/model_page.png)
![Code example of loaded model with textures, using .dae format](./images/model_monster_code.png)
![Code example of loaded model with textures, using .gltf format](./images/model_page_code.png)
### Keyboard/Mouse Controls
The game is controlled using the keyboard and mouse.
![Keyboard/Mouse controls](./images/controls_keyboard.png)
![Keyboard/Mouse controls](./images/controls_mouse.png)
### Audio Playback
The game has background music as well as sound effects for different objects.
![Audio for page collection](./images/audio1.png)
![Audio for monster breathing](./images/audio2.png)
![Audio for player walking](./images/audio3.png)
![Audio for background music](./images/audio4.png)

## Exception Handling

### System/Subsystem Initialisation
![System/Subsystem initialisation](./images/error_handling1.png)
![System/Subsystem initialisation](./images/error_handling2.png)

## Evaluation
I believe that I have created a software prototype of a game that demonstrates a good understanding of writing C++ and OpenGL programs.

I am particularly proud of the basic lighting implementation, as it significantly improves the aesthetics of the game by making colours less flat.

If I were to improve something about this project, I would implement some UI to create visual feedback for when the player collects pages.