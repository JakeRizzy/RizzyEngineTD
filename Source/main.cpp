/*
Product Owner: Jacob Risoldi
Product Name: RizzyEngine
Version: 0.5.0: Geometry Wars Plus
Version Date: May 2026
Language: C++ 20
Toolchain: SFML 2.6.1, ImGui 1.89.9, ImGui-SFML 2.6
*/

#include "Engine.h" // Include the header file that contains the declaration of the Engine class and its member variables and functions.
#include <iostream> // Include the iostream library for using std::cout for printing messages to the console for debugging purposes.

int main() { // Main function that serves as the entry point of the program.
    Engine game("Assets/Levels/config.txt"); // Create an instance of the Engine class, passing in the path to the configuration file for initializing the engine's settings and properties.
    game.run(); // Call the run function of the game instance to start the main game loop and handle the window, rendering, the EntityManager, and all gameplay systems.
	return 0; // Return 0 to indicate that the program has executed successfully and is exiting without errors.
}