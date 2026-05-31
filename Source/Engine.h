#pragma once // Prevents multiple inclusions of this header file.
#include "EntityManager.hpp" // Include the EntityManager header, which manages game entities and their components.
#include <random> // Include the random library for generating safe random numbers when the game is launched.

// Structs to hold the config.txt configuration data
struct PlayerConfig {
	// Player properties: Shape radius, shape vertices, collision radius, shape fill color, shape outline color, shape outline thickness.
	float SOThickness = 0.0f; // Player properties: Shape outline thickness for the player's shape component.
	float SRadius = 0.0f, SCRadius = 0.0f; // Player properties: Shape and Collision radii for the player's shape component.
	int SVertices = 0; // Player properties: Shape vertices for the player's shape component.
	int SFillR = 0, SFillG = 0, SFillB = 0, SFillA = 0; // Player properties: Shape fill color RGBA values for the player's shape component.
	int SOutR = 0, SOutG = 0, SOutB = 0, SOutA = 0; // Player properties: Shape outline color RGBA values for the player's shape component.
	float SSpeed = 0.0f; // Player properties: Shape speed scalar value.
	int SHealth = 0; // Player properties: Health value for the player's health component (determines how many hits the player can take before being destroyed).
};
struct EnemyConfig {
	std::string enemyType; // Enemy properties: A string identifier for the enemy type to differentiate between different enemy configurations.
	float SOThickness = 0.0f; // Enemy properties: Shape outline thickness for the player's shape component.
	float SRadiusMin = 0.0f, SRadiusMax = 0.0f, SCRadius = 0.0f; // Enemy properties: Minimum and maximum values for shape and collision radii.
	int  SVerticesMin = 0, SVerticesMax = 0; // Enemy properties: Minimum and maximum values for shape vertices for the enemy's shape component.
	// Enemy properties: Values for the projectile shape component's fill color RGBA values.
	int SFillR = 0, SFillG = 0, SFillB = 0, SFillA = 0;
	// Enemy properties: Values for the projectile shape component's outline color RGBA values and outline thickness.
	int SOutR = 0, SOutG = 0, SOutB = 0, SOutA = 0;
	// Enemy properties: Minimum and maximum values for the enemy's lifespan component *
	float SLifeMin = 0, SLifeMax = 0;
	float SSpawnMin = 0, SSpawnMax = 0; // Enemy properties: Minimum and maximum values for the enemy's spawn rate *
	int SVal = 0; // Enemy properties: Value for the enemy's score value component (determines how many points the player receives for destroying this enemy).
	float SSpeedMin = 0.0f, SSpeedMax = 0.0f; // Enemy properties: Minimum and maximum values for the enemy's speed scalar value.
	float SHealth = 0.0f; // Enemy properties: Value for the enemy's health component (determines how many projectiles can hit the enemy before it is destroyed).
	float SDamage = 0.0f; // Enemy properties: Value for the enemy's damage component (determines how much damage the enemy will deal to the player upon collision).
	// Enemy properties: A boolean flag to indicate whether the enemy's color properties should be randomized for each spawn.
	bool randomizeColor = false;
	float spawnChance = 0.0f; // Enemy properties: A float value between 0 and 1 to indicate the chance for this enemy type to spawn when an enemy spawn is triggered.
};
struct ProjectileConfig {
	std::string projectileType; // Projectile properties: A string identifier for the projectile type to differentiate between different projectile configurations.
	float SOThickness = 0.0f; // Projectile properties: Shape outline thickness for the player's shape component.
	float SRadius = 0.0f, SCRadius = 0.0f; // Projectile properties: Values for shape and collision radii.
	int  SVerticesMin = 0, SVerticesMax = 0; // Projectile properties: Minimum and maximum values for shape vertices for the enemy's shape component.
	// Projectile properties: Values for the projectile shape component's fill color RGBA values.
	int SFillR = 0, SFillG = 0, SFillB = 0, SFillA = 0;
	// Projectile properties: Values for the projectile shape component's outline color RGBA values and outline thickness.
	int SOutR = 0, SOutG = 0, SOutB = 0, SOutA = 0;
	// Projectile properties: Minimum and maximum values for the projectile's lifespan component *
	float SLifeMin = 0, SLifeMax = 0;
	float SSpawnMin = 0, SSpawnMax = 0; // Projectile properties: Minimum and maximum values for the projectile's spawn rate *
	// Projectile properties: Values for the projectile's score value component (determines how many points the player receives for destroying this projectile if needed).
	int SVal = 0;
	float SSpeedMin = 0.0f, SSpeedMax = 0.0f; // Projectile properties: Minimum and maximum values for the projectile's speed scalar value.
	float SHealth = 0.0f; // Projectile properties: Value for the projectile's health component (determines how many hits the projectile can take before being destroyed if needed).
	float SDamage = 0.0f; // Projectile properties: Value for the projectile's damage component (determines how much damage the projectile will deal to the player or enemies upon collision.
	// Projectile properties: A boolean flag to indicate whether the enemy's color properties should be randomized for each spawn (true).
	bool randomizeColor = false;
};

class Engine { // Defines the Engine class, which is responsible for initializing and running the game, including managing the window, rendering, the EntityManager, and all gameplay systems.
	private: // Private members that are only accessible within the Engine class.
		static std::mt19937 m_rng; // Static member variable for a Mersenne Twister random number generator (RNG) that can be used throughout the Engine class for generating random values.
		sf::RenderWindow m_window; // SFML RenderWindow object for managing the game window and rendering.
		EntityManager m_entities; // EntityManager object for managing game entities and their components.
		sf::Font m_font; // SFML Font object for loading and managing the font used for rendering text in the game.
		sf::Text m_text; // SFML Text object for rendering text in the game, such as the player's score and game state messages.
		sf::Clock m_deltaClock; // SFML Clock object for tracking the time elapsed between frames, used for frame rate independent movement and timing in the game.
		PlayerConfig m_playerConfig; // A struct to hold the player configuration data loaded from the config.txt file.
		std::vector<EnemyConfig> m_enemyConfigs; // A vector of structs to hold multiple enemy configuration datas loaded from the config.txt file for different enemy types.
		std::vector<ProjectileConfig> m_projectileConfigs; // A vector of structs to hold multiple projectile configuration datas loaded from the config.txt file for different projectile types.
		std::string m_lastEnemySubtype = ""; // A string variable to track the subtype of the last enemy spawned, used for limiting consecutive spawns of the same enemy type in a row.
		int m_consecutiveTypeCount = 0; // An integer variable to track the number of consecutive spawns of the same enemy type, used for limiting consecutive spawns of the same enemy type in a row.
		int m_currentScore = 0; // An integer variable to track the player's score in the game, initialized to 0.
		int m_highScore = 0; // An integer variable to track the player's high score, initialized to 0.
		int m_currentFrame = 0; // An integer variable to track the current frame number, initialized to 0.
		int m_currentSpecialAbility = 1; // An integer variable to track the currently selected special ability for the player, initialized to 1.
		float m_deltaTimeSeconds = 0.0f; // A float variable to track the time elapsed between frames in seconds, initialized to 0.
		float m_startTimer = 3.0f; // A float variable to track the countdown timer at the start of the game before the game begins, initialized to 3 seconds.
		float m_gameTime = 0.0f; // A float variable to track the total time elapsed since the start of the current game session in seconds, initialized to 0.
		float m_gameOverTime = 0.0f; // A float variable to track the time elapsed since the game over state was triggered.
		float m_lastPlayerFireTime = -1.5f; // A float variable to track the last time the player fired, initialized to -1.5 so the player can fire right away.
		float m_lastEnemySpawnTime = 0.0f; // A float variable to track the time when the last enemy was spawned, used for timing enemy spawns based on their spawn interval, initialized to 0.
		float m_specialAbilityCooldown = 0.0f; // A float variable to track the amount of time the special ability is on cooldown for.
		float m_specialAbilityDuration = 0.0f; // A float variable to track the amount of time the special ability is active for.
		float m_specialAbilityEnd = 0.0f; // A float variable to track the time when the special ability should end.
		float m_spawnInterval = 0.0f; // A float variable to track the current spawn interval in seconds between spawns, modified by the enemy configuration data, initialized to 0.
		float m_timeSurvived = 0.0f; // A float variable to track the time the player has survived in the current game session, initialized to 0.
		float m_bestTimeSurvived = 0.0f; // A float variable to track the best time the player has survived across all game sessions, initialized to 0.
		bool m_firstEnemySpawned = false; // A boolean flag to indicate whether the first enemy has been spawned yet, used to have the first enemy spawn at a different time than the regular spawn interval.
		bool m_specialAbilityActive = false; // A boolean flag to indicate whether the player's special ability is currently active, initialized to false.
		bool m_mainMenu = true; // A boolean flag to indicate whether the game is currently in the main menu state, initialized to true.
		bool m_paused = false; // A boolean flag to indicate whether the game is currently paused, initialized to false.
		bool m_running = true; // A boolean flag to indicate whether the game loop should continue running, initialized to true.
		bool m_gameOver = false; // A boolean flag to indicate whether the game is over, initialized to false.
		bool m_sMovementEnabled = true; // A boolean flag to indicate whether the movement system should be updated and applied to entities during the game loop.
		bool m_sCollisionEnabled = true; // A boolean flag to indicate whether the collision system should be updated and applied to entities during the game loop.
		bool m_sLifespanEnabled = true; // A boolean flag to indicate whether the lifespan system should be updated and applied to entities during the game loop.
		bool m_sEnemyLogicEnabled = true; // A boolean flag to indicate whether the enemy logic system should be updated and applied to entities during the game loop.
		bool m_sGuiEnabled = false; // A boolean flag to indicate whether the debugging UI should be displayed.


		// Private member function to initialize the game engine, including loading the configuration data from the specified config file path, setting up the window, font, and text objects.
		void init(const std::string& configPath);

		// Systems for handling different aspects of the game, such as input, movement, rendering, collision detection, enemy spawning, scoring, etc..
		void sRender(); // System for rendering all entities with a shape component and rendering the player's score and game state messages using the text component.
		void sMovement(); // System for updating the position of entities based on their velocity and the time elapsed between frames.
		void sCollision(); // System for detecting and handling collisions between entities with collision components, such as player-enemy and player-projectile collisions.
		void sLifespan(); // System for updating the remaining lifespan of entities with a lifespan component and marking them for destruction when their lifespan expires.
		void sInput(); // System for handling player input and updating the player's input component accordingly.
		void sEnemyLogic(); // System for spawning enemies based on their spawn rate and the time elapsed since the last spawn as well as behavior like shooter enemies shooting.
		void sGUI(); // System for rendering the in-game debug UI using ImGui.

		// Helper "Spawner" functions for the Systems to call when they need to spawn new entities based on the configuration data or player input.
		void spawnPlayer(); // Helper function to spawn the player entity at the start of the game.
		void spawnEnemy(); // Helper function to spawn an enemy entity based on the enemy configuration data.
		// Helper function to spawn smaller enemy entities when a larger enemy is destroyed, based on the parent entity's properties and the enemy configuration data.
		void spawnSmallEnemies(std::shared_ptr<Entity> parentEntity);
		// Helper function to spawn a projectile entity based projectile configuration data and the source entity's properties.
		void spawnProjectile(std::shared_ptr<Entity> sourceEntity, const Vec2f& targetPos);
		// Helper function to spawn a special ability entity based on the player's input and the source entity's properties.
		void spawnSpecialAbility(std::shared_ptr<Entity> sourceEntity, int abilityType);

		// Helper functions for managing and loading save data and game states.
		void setPaused(bool paused); // Private member function to set the paused state of the game, which can be called by the input handling system when the player presses the pause key.
		void setGameOver(bool gameOver); // Private member function to set the game over state of the game, which can be called when the player's health reaches 0.
		void restartGame(); // Private member function to reset the game state to start a new game session, which can be called when the player chooses to restart after a game over.
		void saveGameData(); // Private member function to save the current game data to a file, which can be called when the game is paused or exiting.
		void loadSavedData(); // Private member function to load the saved game data from a file, which can be called when starting a new game or resuming a saved game.

		std::shared_ptr<Entity> m_player; // A shared pointer to the player entity, allowing for easy access and management of the player entity throughout the engine.

	public: // Public members that can be accessed from outside the Engine class. Provides the interface for running the game.
		Engine(const std::string& configPath); // Constructor for the Engine class that takes in a configuration file path to initialize the engine's settings and properties.
		void run(); // Public member function to start the main game loop and handle the window, rendering, the EntityManager, and all gameplay systems.
};