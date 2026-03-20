#pragma once // Prevents multiple inclusions of this header file.
#include "EntityManager.hpp" // Include the EntityManager header, which manages game entities and their components.
#include <random>

// Structs to hold the config.txt configuration data
struct PlayerConfig {
	// Player properties: Shape radius, shape vertices, collision radius, shape fill color, shape outline color, shape outline thickness.
	float SOThickness; // Player properties: Shape outline thickness for the player's shape component.
	float SRadius, SCRadius; // Player properties: Shape and Collision radii for the player's shape component.
	int SVertices; // Player properties: Shape vertices for the player's shape component.
	int SFillR, SFillG, SFillB, SFillA; // Player properties: Shape fill color RGBA values for the player's shape component.
	int SOutR, SOutG, SOutB, SOutA; // Player properties: Shape outline color RGBA values for the player's shape component.
	float SSpeed; // Player properties: Shape speed scalar value.
	int SHealth; // Player properties: Health value for the player's health component (determines how many hits the player can take before being destroyed).
};
struct EnemyConfig {
	std::string enemyType; // Enemy properties: A string identifier for the enemy type to differentiate between different enemy configurations.
	float SOThickness; // Enemy properties: Shape outline thickness for the player's shape component.
	float SRadiusMin, SRadiusMax, SCRadiusMin, SCRadiusMax; // Enemy properties: Minimum and maximum values for shape and collision radii for the enemy's shape and collision components.
	int  SVerticesMin, SVerticesMax; // Enemy properties: Minimum and maximum values for shape vertices for the enemy's shape component.
	// Enemy properties: Values for the projectile shape component's fill color RGBA values.
	int SFillR, SFillG, SFillB, SFillA;
	// Enemy properties: Values for the projectile shape component's outline color RGBA values and outline thickness.
	int SOutR, SOutG, SOutB, SOutA;
	float SLifeMin, SLifeMax; // Enemy properties: Minimum and maximum values for the enemy's lifespan component in frames (determines how long the enemy will exist before being automatically destroyed).
	int SSpawnMin, SSpawnMax; // Enemy properties: Minimum and maximum values for the enemy's spawn rate in frames (determines how often this enemy type will spawn).
	int SValMin, SValMax; // Enemy properties: Minimum and maximum values for the enemy's score value component (determines how many points the player receives for destroying this enemy).
	float SSpeedMin, SSpeedMax; // Enemy properties: Minimum and maximum values for the enemy's speed scalar value.
	int SHealth; // Enemy properties: Value for the enemy's health component (determines how many projectiles can hit the enemy before it is destroyed).
	int SDamage; // Enemy properties: Value for the enemy's damage component (determines how much damage the enemy will deal to the player upon collision).
	// Enemy properties: A boolean flag to indicate whether the enemy's color properties should be randomized for each spawn (true).
	bool randomizeColor;
};
struct ProjectileConfig {
	std::string projectileType; // Projectile properties: A string identifier for the projectile type to differentiate between different projectile configurations.
	float SOThickness; // Projectile properties: Shape outline thickness for the player's shape component.
	float SRadiusMin, SRadiusMax, SCRadiusMin, SCRadiusMax; // Projectile properties: Minimum and maximum values for shape and collision radii for the enemy's shape and collision components.
	int  SVerticesMin, SVerticesMax; // Projectile properties: Minimum and maximum values for shape vertices for the enemy's shape component.
	// Projectile properties: Values for the projectile shape component's fill color RGBA values.
	int SFillR, SFillG, SFillB, SFillA;
	// Projectile properties: Values for the projectile shape component's outline color RGBA values and outline thickness.
	int SOutR, SOutG, SOutB, SOutA;
	float SLifeMin, SLifeMax; // Enemy properties: Minimum and maximum values for the enemy's lifespan component in frames (determines how long the enemy will exist before being automatically destroyed).
	int SSpawnMin, SSpawnMax; // Enemy properties: Minimum and maximum values for the enemy's spawn rate in frames (determines how often this enemy type will spawn).
	// Projectile properties: Minimum and maximum values for the projectile's score value component (determines how many points the player receives for destroying this projectile if needed).
	int SValMin, SValMax;
	float SSpeedMin, SSpeedMax; // Projectile properties: Minimum and maximum values for the projectile's speed scalar value.
	int SHealth; // Projectile properties: Value for the projectile's health component (determines how many hits the projectile can take before being destroyed if needed).
	int SDamage; // Projectile properties: Value for the projectile's damage component (determines how much damage the projectile will deal to the player or enemies upon collision.
	// Enemy properties: A boolean flag to indicate whether the enemy's color properties should be randomized for each spawn (true).
	bool randomizeColor;
};

class Engine { // Defines the Engine class, which is responsible for initializing and running the game, including managing the window, rendering, the EntityManager, and all gameplay systems.
	private: // Private members that are only accessible within the Engine class.
		static std::mt19937 m_rng; 
		sf::RenderWindow m_window; // SFML RenderWindow object for managing the game window and rendering.
		EntityManager m_entities; // EntityManager object for managing game entities and their components.
		sf::Font m_font; // SFML Font object for loading and managing the font used for rendering text in the game.
		sf::Text m_text; // SFML Text object for rendering text in the game, such as the player's score and game state messages.
		PlayerConfig m_playerConfig; // A struct to hold the player configuration data loaded from the config.txt file.
		std::vector<EnemyConfig> m_enemyConfigs; // A vector of structs to hold multiple enemy configuration data loaded from the config.txt file for different enemy types.
		std::vector<ProjectileConfig> m_projectileConfigs; // A vector of structs to hold multiple projectile configuration data loaded from the config.txt file for different projectile types.
		sf::Clock m_deltaClock; // SFML Clock object for tracking the time elapsed between frames, used for frame rate independent movement and timing in the game.
		int m_currentScore = 0; // An integer variable to track the player's score in the game, initialized to 0.
		int m_highScore = 0; // An integer variable to track the player's high score, initialized to 0.
		int m_frameCount = 0; // An integer variable to track the number of frames that have elapsed since the start of the game, initialized to 0.
		int m_currentFrame = 0; // An integer variable to track the current frame number, used for timing enemy and projectile spawns based on their spawn rate, initialized to 0.
		int m_lastEnemySpawnFrame = 0; // An integer variable to track the frame number when the last enemy was spawned, used for timing enemy spawns based on their spawn interval, initialized to 0.
		bool m_paused = false; // A boolean flag to indicate whether the game is currently paused, initialized to false.
		bool m_running = true; // A boolean flag to indicate whether the game loop should continue running, initialized to true.
		bool m_gameOver = false; // A boolean flag to indicate whether the game is over, initialized to false.

		// Private member function to initialize the game engine, including loading the configuration data from the specified config file path, setting up the window, font, and text objects.
		void init(const std::string& configPath);
		void setPaused(bool paused); // Private member function to set the paused state of the game, which can be called by the input handling system when the player presses the pause key.
		void setGameOver(bool gameOver); // Private member function to set the game over state of the game, which can be called when the player's health reaches 0.

		// Systems for handling different aspects of the game, such as input, movement, rendering, collision detection, enemy spawning, scoring, etc..
		void sRender(); // System for rendering all entities with a shape component and rendering the player's score and game state messages using the text component.
		void sMovement(); // System for updating the position of entities based on their velocity and the time elapsed between frames.
		void sCollision(); // System for detecting and handling collisions between entities with collision components, such as player-enemy and player-projectile collisions.
		void sLifespan(); // System for updating the remaining lifespan of entities with a lifespan component and marking them for destruction when their lifespan expires.
		void sInput(); // System for handling player input and updating the player's input component accordingly.
		void sEnemySpawner(); // System for spawning enemies based on their spawn rate and the time elapsed since the last spawn, using the enemy configuration data.
		/* not used at this time:
		void sProjectileSpawner(); // System for spawning projectiles based on their spawn rate and the time elapsed since the last spawn, using the projectile configuration data.
		*/
		void sGUI(); // System for rendering the in-game debug UI using ImGui.

		// Helper "Spawner" functions for the Systems to call when they need to spawn new entities based on the configuration data or player input.
		void spawnPlayer(); // Helper function to spawn the player entity at the start of the game.
		void spawnEnemy(); // Helper function to spawn an enemy entity based on the enemy configuration data.
		// Helper function to spawn smaller enemy entities when a larger enemy is destroyed, based on the parent entity's properties and the enemy configuration data.
		void spawnSmallEnemies(std::shared_ptr<Entity> parentEntity);
		// Helper function to spawn a projectile entity based on the player's input, the mouse position, and the projectile configuration data.
		void spawnProjectile(std::shared_ptr<Entity> sourceEntity, const Vec2f& mousePos);
		// Helper function to spawn a special ability entity based on the player's input and the source entity's properties.
		void spawnSpecialAbility(std::shared_ptr<Entity> sourceEntity);

		std::shared_ptr<Entity> m_player; // A shared pointer to the player entity, allowing for easy access and management of the player entity throughout the engine.

	public: // Public members that can be accessed from outside the Engine class. Provides the interface for running the game.
		Engine(const std::string& configPath); // Constructor for the Engine class that takes in a configuration file path to initialize the engine's settings and properties.
		void run(); // Public member function to start the main game loop and handle the window, rendering, the EntityManager, and all gameplay systems.
};