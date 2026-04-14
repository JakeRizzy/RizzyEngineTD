/*
Product Owner: Jacob Risoldi
Product Name: RizzyEngine
Version: 0.5.0: Geometry Wars Plus
Version Date: April 2026
Language: C++ 20
Toolchain: SFML 2.6.1, ImGui 1.89.9, ImGui-SFML 2.6
*/

#include "Engine.h"
#include <iostream>

int main() {
    Engine game("config.txt"); // Create an instance of the Engine class, passing in the path to the configuration file for initializing the engine's settings and properties.
    game.run(); // Call the run function of the game instance to start the main game loop and handle the window, rendering, the EntityManager, and all gameplay systems.
    return 0;
}