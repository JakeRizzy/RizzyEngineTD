int main(){ // The entry point into the game engine.
	Engine engine("config.txt"); // Create an instance of the Engine class, passing in the path to the configuration file for initializing the engine's settings and properties.
	engine.run(); // Call the run function of the Engine instance to start the main game loop and handle the window, rendering, the EntityManager, and all gameplay systems.
	return 0; // Return 0 to indicate that the program ended successfully.
}