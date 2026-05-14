#include "Engine.h" // Include the header file that contains the declaration of the Engine class and its variables and functions.
#include <fstream> // Include the fstream library for file input and output operations. Required for loading the configuration file (config.txt) in the Engine constructor.
#include <iostream> // Include the iostream library for input and output operations. Required for printing error messages to the console.
#include <iomanip> // Include the iomanip library for input and output manipulators. Required for formatting the output of the player's score and game state messages in the HUD.
#include <sstream> // Include the sstream library for string stream operations. Required for constructing formatted strings for the HUD display of the player's score and game state messages.
#include <filesystem> // Include the filesystem library for handling file paths and checking file existence. Required for managing save and configuration files.
#include <cstdio> // Include the cstdio library for C-style input and output operations. Required for file handling and data management.
#include "imgui.h" // Include the ImGui library for creating graphical user interfaces (GUI) in the game. Required for rendering the in-game debug UI.
#include "imgui-SFML.h" // Include the ImGui-SFML binding library for integrating ImGui with SFML. Required for rendering ImGui interfaces within the SFML window.
// Initialize the static member
std::mt19937 Engine::m_rng;

// Implementation of the Engine constructor, which initializes the game engine and loads the configuration data from the specified config file path.
Engine::Engine(const std::string& configPath) {
	init(configPath); // Call the private init function to initialize the engine's settings and properties based on the configuration file.
}

// Implementation of the private init function for the Engine class, which initializes the engine's settings and properties based on the configuration file.
void Engine::init(const std::string& configPath) {
	// Seed the static RNG once
	std::random_device rd;
	m_rng.seed(rd());

	// Config File Parsing:
	std::ifstream fin(configPath); // Create an input file stream (ifstream) object to read from the configuration file specified by configPath.
	std::string type; // Declare a string variable to hold the type of configuration being read (e.g.,"Window", "Player", "Enemy", "Projectile").
	std::string subtype; // Declare a string variable to hold the subtype of the configuration being read (e.g., Enemy Tank or Projectile PPlayer).

	if (fin.is_open()) { // Check if the file stream was successfully opened.
		while (fin >> type) { // Read the first word from the file into the 'type' variable, which indicates the type of configuration being read.
			
			// Skip comment lines in the config file that start with "#" by checking if the first character is "#". If it is, ignore the rest of the line and continue to read the next line.
			if (type[0] == '#') {
				std::string skipLine; // Declare a string variable to hold the rest of the line that is being skipped.
				std::getline(fin, skipLine); // Ignore the rest of the line
				continue; // Continue to the next iteration of the loop
			}

			// Read in window configuration values and set up the SFML RenderWindow accordingly.
			if (type == "Window") { // Check if the type of configuration being read is "Window".
				int width, height, fullscreen; // Declare integer variables to hold the window configuration values read from the file.
				std::string windowTitle; // Declare a string variable to hold the window title read from the file.
				fin >> width >> height >> fullscreen; // Read in window configuration values from the file, including width, height fullscreen flag.
				std::getline(fin, windowTitle); // Read the rest of the line for the window title, allowing for spaces in the title.

				if (width == 0 || height == 0) { // Check if the width or height values read from the config file are 0, which would be invalid for creating a window. If either value is 0,
					sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode(); // Then get the desktop video mode using SFML's VideoMode class to use the desktop resolution for calculating window size.
					width = desktopMode.width / 1.5f; // Set the width to the desktop width divided by 1.5 to create a window that is slightly smaller than the full desktop resolution.
					height = desktopMode.height / 1.5f; // Set the height to the desktop height divided by 1.5 to create a window that is slightly smaller than the full desktop resolution.
				}
				else if (fullscreen) { // Check if the fullscreen flag is set in the config file. If it is,
					m_window.create(sf::VideoMode(width, height), windowTitle, sf::Style::Fullscreen); // Then create a fullscreen window using the SFML RenderWindow's create function.
				}
				else {
					m_window.create(sf::VideoMode(width, height), windowTitle); // Create a windowed mode window using the SFML RenderWindow's create function.
				}
				m_window.setVerticalSyncEnabled(true); // Enable vertical sync for the window to synchronize the frame rate with the monitor's refresh rate, instead of hardcoding a frame rate limit.
				
				// Initialize ImGui-SFML with the created window, allowing for rendering of ImGui interfaces within the SFML window.
				if (!ImGui::SFML::Init(m_window)) { // Check if the initialization of ImGui-SFML was successful. If it failed,
					std::cerr << "Failed to initialize ImGui-SFML\n"; // Then print an error message to the console indicating the failure to initialize ImGui-SFML.
				}
			}
			// Read in font configuration values and load the font using the SFML Font object.
			else if (type == "Font") {
				std::string path; int size, r, g, b; // Declare variables to hold the font configuration values read from the file.
				fin >> path >> size >> r >> g >> b; // Read in font configuration values from the file, including the file path to the font, font size, and font color RGBA values.
				if (m_font.loadFromFile(path)) { // Attempt to load the font from the specified file path using the SFML Font object's loadFromFile function. If it succeeds,
					m_text.setFont(m_font); // Set the font of the SFML Text object to the loaded font for rendering text in the game.
					m_text.setCharacterSize(size); // Set the character size of the SFML Text object based on the config file value for rendering text in the game.
					m_text.setFillColor(sf::Color(r, g, b)); // Set the fill color of the SFML Text object based on the config file RGBA values for rendering text in the game.
				}
				else { // If the font failed to load from the specified file path,
					std::cerr << "Failed to load font from path: " << path << "\n"; // Then print an error message to the console indicating the failure to load the font.
				}
			}
			// Read in player configuration values and store them in the m_playerConfig struct for later use when spawning the player entity.
			else if (type == "Player") {
				fin >> m_playerConfig.SOThickness
					>> m_playerConfig.SRadius >> m_playerConfig.SCRadius
					>> m_playerConfig.SVertices
					>> m_playerConfig.SFillR >> m_playerConfig.SFillG >> m_playerConfig.SFillB >> m_playerConfig.SFillA
					>> m_playerConfig.SOutR >> m_playerConfig.SOutG >> m_playerConfig.SOutB >> m_playerConfig.SOutA
					>> m_playerConfig.SSpeed
					>> m_playerConfig.SHealth;
			}
			// Read in enemy configuration values and store them in an EnemyConfig struct, then add that struct to the m_enemyConfigs vector for later use when spawning enemy entities.
			else if (type == "Enemy") {
				EnemyConfig enemyConfig; // Create a temporary EnemyConfig struct to hold the configuration values for the current enemy type being read from the file.
				int randColFlag; // Declare an integer variable to hold the randomize color flag value read from the file, which will be converted to a boolean value for the EnemyConfig struct.
				fin >> enemyConfig.enemyType
					>> enemyConfig.SOThickness
					>> enemyConfig.SRadiusMin >> enemyConfig.SRadiusMax >> enemyConfig.SCRadius
					>> enemyConfig.SVerticesMin >> enemyConfig.SVerticesMax
					>> enemyConfig.SFillR >> enemyConfig.SFillG >> enemyConfig.SFillB >> enemyConfig.SFillA
					>> enemyConfig.SOutR >> enemyConfig.SOutG >> enemyConfig.SOutB >> enemyConfig.SOutA
					>> enemyConfig.SLifeMin >> enemyConfig.SLifeMax
					>> enemyConfig.SSpawnMin >> enemyConfig.SSpawnMax
					>> enemyConfig.SVal
					>> enemyConfig.SSpeedMin >> enemyConfig.SSpeedMax
					>> enemyConfig.SHealth
					>> enemyConfig.SDamage
					>> randColFlag
					>> enemyConfig.spawnChance;
				enemyConfig.randomizeColor = static_cast<bool>(randColFlag); // Convert the integer randomize color flag to a boolean value and store it in the EnemyConfig struct.
				m_enemyConfigs.push_back(enemyConfig); // Add the configured EnemyConfig struct to the m_enemyConfigs vector for later use when spawning enemy entities.
			}
			// Read in projectile configuration values and store them in a ProjectileConfig struct, then add that struct to the m_projectileConfigs vector for later use when spawning projectile entities.
			else if (type == "Projectile") {
				ProjectileConfig projectileConfig; // Create a temporary ProjectileConfig struct to hold the configuration values for the current projectile type being read from the file.
				int randColFlag; // Declare an integer variable to hold the randomize color flag value read from the file, which will be converted to a boolean value for the ProjectileConfig struct.
				fin >> projectileConfig.projectileType
					>> projectileConfig.SOThickness
					>> projectileConfig.SRadius >> projectileConfig.SCRadius
					>> projectileConfig.SVerticesMin >> projectileConfig.SVerticesMax
					>> projectileConfig.SFillR >> projectileConfig.SFillG >> projectileConfig.SFillB >> projectileConfig.SFillA
					>> projectileConfig.SOutR >> projectileConfig.SOutG >> projectileConfig.SOutB >> projectileConfig.SOutA
					>> projectileConfig.SLifeMin >> projectileConfig.SLifeMax
					>> projectileConfig.SSpawnMin >> projectileConfig.SSpawnMax
					>> projectileConfig.SVal
					>> projectileConfig.SSpeedMin >> projectileConfig.SSpeedMax
					>> projectileConfig.SHealth
					>> projectileConfig.SDamage
					>> randColFlag;
				projectileConfig.randomizeColor = static_cast<bool>(randColFlag); // Convert the integer randomize color flag to a boolean value and store it in the ProjectileConfig struct.
				m_projectileConfigs.push_back(projectileConfig); // Add the configured ProjectileConfig struct to the m_projectileConfigs vector for later use when spawning projectile entities.
			}
			else {
				std::cerr << "Unknown config type: " << type << " - Skipping.\n"; // Print an error message to the console if an unknown config type is encountered in the file, but continue.
				std::string skipLine; // Declare a string variable to hold lines that need to be skipped due to unknown config types.
				std::getline(fin, skipLine); // Read and discard the rest of the line after an unknown config type is encountered to move on to the next line of valid config data.
			}
		}
		fin.close(); // Close the file stream after reading all configuration data from the file.
	}
	// Validate that all critical configurations were loaded before attempting to start the game.
	if (!m_window.isOpen()) { // If no Window config was loaded, the window was never created and the game cannot run.
		std::cerr << "No Window configuration found in config file. Terminating.\n";
		m_running = false; // Set running to false so the game loop exits immediately without crashing.
		return;
	}
	if (m_projectileConfigs.empty()) { // If no Projectile configurations were loaded, the game cannot function since shooting is a core mechanic.
		std::cerr << "No Projectile configurations found in config file. Terminating.\n";
		m_running = false;
		return;
	}
	if (m_enemyConfigs.empty()) { // If no Enemy configurations were loaded, the game cannot function since enemies are a core mechanic.
		std::cerr << "No Enemy configurations found in config file. Terminating.\n";
		m_running = false;
		return;
	}
	m_spawnInterval = m_enemyConfigs[0].SSpawnMax; // Set the initial spawn interval to the maximum value from the first enemy config;
	spawnPlayer(); // Call the helper function to spawn the player entity at the start of the game.
	loadSavedData(); // Call the helper function to load saved game data from a file.
}

void Engine::run() { // Implementation of the run function, which contains the main game loop that updates and renders the game until it is closed.
	while (m_running) { // Main game loop that continues running until the m_running flag is set to false, which can happen when the player closes the window or when the game is otherwise terminated.
		m_entities.update(); // Call the EntityManager's update function at the start of each frame to add any pending entities from the "Waiting Room".
		// Initialize an SFML Time object called deltaTime to hold the time elapsed since the last frame by restarting the m_deltaClock and getting the elapsed time every frame.
		sf::Time deltaTime = m_deltaClock.restart(); // Restart the m_deltaClock to get the time elapsed since the last frame.
		m_deltaTimeSeconds = deltaTime.asSeconds(); // Convert the elapsed time to seconds and store it in m_deltaTimeSeconds.
		ImGui::SFML::Update(m_window, deltaTime); // Update the ImGui-SFML integration with the current window and delta time to allow for rendering of ImGui interfaces within the SFML window.
		
		sInput(); // Call the input handling system to process player input and update the game state accordingly.

		if (!m_paused && !m_gameOver) { // If the game is not currently paused and not currently over, then
			if (m_sEnemyLogicEnabled) sEnemyLogic(); // If the enemy logic system is enabled, call the enemy logic system to handle enemy behavior,
			if (m_sMovementEnabled) sMovement(); // If the movement system is enabled, call the movement system to update entity positions,
			if (m_sCollisionEnabled) sCollision(); // If the collision system is enabled, call the collision system to check for and resolve collisions between entities,
			if (m_sLifespanEnabled) sLifespan(); // If the lifespan system is enabled, call the lifespan system to update entity lifespans and destroy entities that have exceeded their lifespan,
			m_currentFrame++; // increment the current frame counter by 1, every frame that the game is not paused or over, to keep track of how many frames have elapsed,
			m_gameTime += m_deltaTimeSeconds; // increment the total game time by the time elapsed since the last frame to keep track of the total game time.
			m_timeSurvived = m_gameTime; // Update the time survived in this session to match the total game time.
			if (m_currentScore > m_highScore) m_highScore = m_currentScore; // Update the displayed high score in real time so the HUD always shows the running maximum, not just the value at the last save point.
			if (m_timeSurvived > m_bestTimeSurvived) m_bestTimeSurvived = m_timeSurvived; // Update the displayed best time in real time so the HUD always reflects the current session if it's the longest.
			m_specialAbilityCooldown = m_specialAbilityCooldown - m_deltaTimeSeconds; // Decrease the special ability cooldown by the time elapsed.
			m_specialAbilityDuration = m_specialAbilityDuration - m_deltaTimeSeconds; // Decrease the special ability duration by the time elapsed.
			if (m_specialAbilityDuration <= 0.0f) { // If the special ability duration has reached zero or below,
				m_specialAbilityActive = false;  // then set the special ability active flag to false,
				m_specialAbilityDuration = 0.0f; // and reset the special ability duration to zero to avoid negative values.
			}
			if (m_specialAbilityCooldown <= 0.0f) m_specialAbilityCooldown = 0.0f; // If the special ability cooldown has reached zero or below, reset it to zero to avoid negative values.
		}
		else if (m_gameOver) { // If the game is currently over, then
			m_gameOverTime += m_deltaTimeSeconds; // Increment the game over timer by the time elapsed since the last frame to keep track of how long the game has been in the game over state.
		}

		if (m_sGuiEnabled) sGUI(); // If the GUI system is enabled, call the GUI system to render the in-game debug UI with ImGui.
		
		sRender(); // Call the rendering system to draw all entities and the GUI to the window.
	}
	saveGameData(); // Save the game data to a file when the game loop ends to ensure that progress is saved even if the player exits abruptly without pausing or reaching a game over state.
	ImGui::SFML::Shutdown(); // Shutdown the ImGui-SFML integration when the game loop ends.
}

void Engine::setPaused(bool paused) { // Implementation of the setPaused function, which sets the paused state of the game.
	m_paused = paused; // Set the m_paused flag to the provided value to indicate whether the game is currently paused or not.
	if (paused) { // If the game is now paused, then
		// check if the time survived in this session is greater than the best time survived across all sessions,
		if (m_timeSurvived > m_bestTimeSurvived) m_bestTimeSurvived = m_timeSurvived; // if it is, update the best time survived to the current session's time survived.
		// check if the player's score in this session is greater than the high score across all sessions,
		if (m_currentScore > m_highScore) m_highScore = m_currentScore; // if it is, update the high score to the current session's score.
		saveGameData(); // Save the game data to a file to persist across game sessions, immediately after the game is paused to ensure saving even if the player exits while paused.
	}
}

void Engine::setGameOver(bool gameOver) { // Implementation of the setGameOver function, which sets the game over state of the game.
	m_gameOver = gameOver; // Set the m_gameOver flag to the provided value to indicate whether the game is currently over or not.
	if (gameOver) { // If the game is now over, then
		m_gameOverTime = 0.0f; // Reset the game over timer to zero to start counting the time since the game over state was triggered.
		// check if the time survived in this session is greater than the best time survived across all sessions,
		if (m_timeSurvived > m_bestTimeSurvived) m_bestTimeSurvived = m_timeSurvived; // if it is, update the best time survived to the current session's time survived.
		// check if the player's score in this session is greater than the high score across all sessions,
		if (m_currentScore > m_highScore) m_highScore = m_currentScore; // if it is, update the high score to the current session's score.
		saveGameData(); // Save the game data to a file to persist across game sessions, immediately after the game is over to ensure saving even if te player exits immediately.
	}
}

void Engine::restartGame() {
	for (auto& e : m_entities.getEntities()) { // Loop through all entities currently tracked by the EntityManager.
		e->destroy(); // Call the destroy function on each entity, which sets their active flag to false and marks them for removal by the EntityManager's update function.
	}
	m_entities.update(); // Call the EntityManager's update function to remove all entities that were just marked as inactive by the destroy calls, effectively clearing the game world.

	m_currentScore = 0; // Reset the player's score to zero for the new session.
	m_gameTime = 0.0f; // Reset the gameplay clock so all spawn and cooldown timers start fresh.
	m_timeSurvived = 0.0f; // Reset the time survived for this session to zero.
	m_lastEnemySpawnTime = 0.0f; // Reset the last enemy spawn time to zero so that enemies will start spawning immediately based on their spawn intervals.
	m_spawnInterval = m_enemyConfigs[0].SSpawnMax; // Reset the spawn interval to the initial max spawn interval from the first enemy config.
	m_firstEnemySpawned = false; // Reset the flag that tracks whether the first enemy has been spawned to false so that the game can properly handle the timing of the first enemy spawn.
	m_lastEnemySubtype = ""; // Reset the last enemy subtype string to an empty string to allow any enemy type to be spawned first in the new session without restrictions based on the last spawn.
	m_consecutiveTypeCount = 0; // Reset the consecutive spawn count to zero to allow any enemy type to be spawned without restrictions at the start of the new session.
	m_lastPlayerFireTime = -1.5f; // Reset the last player fire time to -1.5 seconds so the player can fire immediately at the start of the new session without waiting for a cooldown.
	m_specialAbilityActive = false; // Reset the special ability active flag to false to ensure that no special ability effects are active at the start of the new session.
	m_specialAbilityCooldown = 0.0f; // Reset the special ability cooldown to zero so the player can use their special ability immediately at the start of the new session.
	m_specialAbilityDuration = 0.0f; // Reset the special ability duration to zero so that any active special ability effects will end immediately at the start of the new session.
	m_specialAbilityEnd = 0.0f; // Reset the special ability end time to zero to ensure that any special ability effects will end immediately at the start of the new session.
	m_currentSpecialAbility = 1; // Reset the currently selected special ability to the default value of 1 at the start of the new session.
	m_gameOver = false; // Reset the game over flag to false to allow the game loop to run and the player to play the new session.
	m_paused = false; // Reset the paused flag to false to ensure the game is not paused at the start of the new session.

	spawnPlayer(); // Call the helper function to spawn a new player entity at the center of the window for the new session.
}

void Engine::saveGameData() {
	std::filesystem::create_directories("AppData"); // Create the "AppData" directory if it does not already exist to ensure there is a location to save the game data file.
	std::ofstream file("AppData/SavedGame.dat"); // Create or open an output file stream object to write to a file named "SavedGame.dat" in the "AppData" directory for saving the game data.
	if (file.is_open()) { // Check if the file stream was successfully opened for writing.
		file << "HighScore " << m_highScore << "\n"; // Write the current high score to the file with a label for identification when loading.
		file << "BestTimeSurvived " << m_bestTimeSurvived << "\n"; // Write the best time survived to the file with a label for identification when loading.
	}
}

void Engine::loadSavedData() {
	std::ifstream file("AppData/SavedGame.dat"); // Create an input file stream object to read from the "SavedGame.dat" file that contains the saved game data from previous sessions.
	if (file.is_open()) { // Check if the file stream was successfully opened for reading.
		std::string label; // Declare a string variable to hold the label read from the file, which indicates what type of data is being read (e.g., "HighScore", "BestTimeSurvived").
		file >> label >> m_highScore; // Read the first label and the high score value from the file. The label is read to ensure that the correct data is being loaded into the correct variable.
		file >> label >> m_bestTimeSurvived; // Read the second label and the best time survived value from the file.
	}
}

void Engine::spawnPlayer() {
	// Create a new entity with the tag "Player" using the EntityManager's createEntity function, which returns a shared pointer to the newly created entity.
	auto player = m_entities.createEntity("player");

	float centerX = m_window.getSize().x / 2.0f; // Calculate the center X coordinate of the window by dividing the window's width by 2, and store it.
	float centerY = m_window.getSize().y / 2.0f; // Calculate the center Y coordinate of the window by dividing the window's height by 2, and store it.
	
	// Add a CTransform component to the player entity with the initial position set to the center of the window, zero velocity, and an angle of 0 degrees.
	player->add<CTransform>(Vec2f(centerX, centerY), Vec2f(0.0f, 0.0f), 0.0f, 0.0f);
	// Add a CShape component to the player entity with the properties defined in the m_playerConfig struct, which was loaded from the config file.
	player->add<CShape>(
		m_playerConfig.SOThickness,
		m_playerConfig.SRadius,
		m_playerConfig.SVertices,
		sf::Color(m_playerConfig.SFillR, m_playerConfig.SFillG, m_playerConfig.SFillB, m_playerConfig.SFillA),
		sf::Color(m_playerConfig.SOutR, m_playerConfig.SOutG, m_playerConfig.SOutB, m_playerConfig.SOutA)
	);
	// Add a CCollision component to the player entity with the collision radius defined in the m_playerConfig struct, which was loaded from the config file.
	player->add<CCollision>(m_playerConfig.SCRadius);
	// Add a CHealth component to the player entity with the health value defined in the m_playerConfig struct, which was loaded from the config file.
	player->add<CHealth>(static_cast<float>(m_playerConfig.SHealth));
	// Add a CInput component to the player entity to handle player input.
	player->add<CInput>();
	// Add a CSpecialAbility component to the player entity with the initial special ability type set to 1 and a cooldown of 10 seconds.
	player->add<CSpecialAbility>(1, 10.0f);

	m_player = player; // Store a reference to the player entity in the Engine class for easy access when updating the player's state based on input and collisions during the game loop.
}

void Engine::spawnEnemy() {
	// Randomly select an enemy type to spawn using a weighted random selection method based on the spawnChance values defined in the enemy configuration data.
	std::uniform_real_distribution<float> spawnChanceDist(0.0f, 1.0f); // Create a uniform real distribution for generating random spawn chance values between 0 and 1.
	float spawnValue = spawnChanceDist(m_rng); // Generate a random value using the spawn chance distribution to determine which enemy type to spawn based on their defined spawn chances.

	// Select the enemy type to spawn based on the generated spawn value.
	EnemyConfig* selectedConfig = nullptr; // Initialize a pointer to hold the selected enemy configuration for the enemy type that will be spawned.
	float cumulativeChance = 0.0f; // Initialize a variable to keep track of the cumulative spawn chance as we iterate through the enemy configurations.

	// Iterate through the enemy configurations and add up their spawn chances until we find the first configuration where the cumulative chance exceeds the generated spawn value.
	for (auto& config : m_enemyConfigs) { // Iterate through each enemy configuration in the m_enemyConfigs vector.
		cumulativeChance += config.spawnChance; // Add the current enemy configuration's spawn chance to the cumulative chance.
		if (spawnValue <= cumulativeChance) { // Check if the generated spawn value is less than or equal to the cumulative chance. If it is, this is the enemy type that will be spawned.
			selectedConfig = &config; // Set the selectedConfig pointer to point to the current enemy configuration.
			break; // Break out of the loop since we have found the enemy type to spawn.
		}
	}
	
	// If no enemy configuration was selected based on the spawn chances (which could happen if the spawn chances do not sum to 1), default to the first enemy configuration in the vector.
	if (!selectedConfig) selectedConfig = &m_enemyConfigs[0];
	
	// --- CONSECUTIVE SUBTYPE LIMITER---
		if (selectedConfig->enemyType == m_lastEnemySubtype) { // Check if the enemy type of the selected configuration is the same as the last enemy subtype that was spawned. If it is, then
			m_consecutiveTypeCount++; // increment the consecutive type count by 1 to track how many times this enemy subtype has been spawned in a row.
		}
		else { // If the selected enemy type is different from the last spawned subtype,
			m_lastEnemySubtype = selectedConfig->enemyType; // then update the last enemy subtype to the newly selected enemy type since we are now spawning a different subtype,
			m_consecutiveTypeCount = 1; // and reset the consecutive type count to 1 since we have just spawned one of this new subtype, starting a new count for this subtype.
		}
	// If we've spawned this exact type 3 times in a row (> 2),
	if (m_consecutiveTypeCount > 2) {
		selectedConfig = &m_enemyConfigs[0]; // then force the spawn of the first enemy type in the configuration list by setting the selectedConfig pointer to point to the first enemy configuration,
		m_lastEnemySubtype = selectedConfig->enemyType; // and reset the last enemy subtype to match this forced spawn type to ensure the consecutive spawn logic continues,
		m_consecutiveTypeCount = 1; // and reset the consecutive type count to 1 since we are now spawning a different subtype, starting a new count for this subtype.
	}
	auto& enemyConfig = *selectedConfig; // Create a reference to the selected enemy configuration for easier access to its properties when spawning the enemy entity.
	
	float outlineThickness = enemyConfig.SOThickness; // Get the shape outline thickness for the enemy from the config file for the selected enemy type.

	// Define distributions for the enemy's properties based on the enemy subtype configuration values.
	std::uniform_real_distribution<float> distRadius(enemyConfig.SRadiusMin, enemyConfig.SRadiusMax);
	float radius = distRadius(m_rng); // Generate a random radius for the enemy using the defined distribution based on the enemy configuration values.

	float totalSize = radius + outlineThickness; // Calculate the total size of the enemy based on its radius and outline thickness.

	std::uniform_int_distribution<int> distVertices(enemyConfig.SVerticesMin, enemyConfig.SVerticesMax);
	int vertices = distVertices(m_rng); // Generate a random number of vertices for the enemy's shape using the defined distribution based on the enemy configuration values.

	std::vector<sf::Color> reservedColors; // Create a vector to hold reserved colors that should not be used when randomizing the enemy's color.
	reservedColors.push_back(sf::Color(m_playerConfig.SFillR, m_playerConfig.SFillG, m_playerConfig.SFillB, m_playerConfig.SFillA)); // Add the player's fill color to the reserved colors vector.
	for (const auto& config : m_enemyConfigs) { // Iterate through the enemy configurations and add their fill colors to the reserved colors vector.
		if (!config.randomizeColor) { // Check if the enemy configuration specifies that the enemy's color should not be randomized.
			reservedColors.push_back(sf::Color(config.SFillR, config.SFillG, config.SFillB, config.SFillA)); // Then add the enemy's fill color to the reserved colors vector.
		}
	}

	sf::Color fillColor; // Declare a variable to hold the enemy's fill color.
	int r, g, b, a; // Declare variables to hold the RGBA components of the enemy's fill color when randomizing.
	std::uniform_int_distribution<int> distColor(0, 255);
	if (!enemyConfig.randomizeColor) { // Check if the enemy configuration specifies that the enemy's color should not be randomized.
		r = enemyConfig.SFillR; // Get the red component of the enemy's fill color from the config file for the selected enemy type.
		g = enemyConfig.SFillG; // Get the green component of the enemy's fill color from the config file for the selected enemy type.
		b = enemyConfig.SFillB; // Get the blue component of the enemy's fill color from the config file for the selected enemy type.
		a = enemyConfig.SFillA; // Get the alpha component of the enemy's fill color from the config file for the selected enemy type.
		fillColor = sf::Color(r, g, b, a); // Create an SFML Color object for the enemy's fill color using the RGBA values from the enemy configuration.
	}
	else { // If the enemy's color should be randomized, generate random RGB values for the enemy's fill color while ensuring that the generated color is not too close to a reserved color.
		bool validColor = false; // Initialize a flag to indicate whether a valid color has been generated that is not too close to any reserved colors, set to false initially to generate.
		while (!validColor) { // Loop until a valid color is generated.
			r = distColor(m_rng); // Generate a random red value for the enemy's fill color using the defined distribution.
			g = distColor(m_rng); // Generate a random green value for the enemy's fill color using the defined distribution.
			b = distColor(m_rng); // Generate a random blue value for the enemy's fill color using the defined distribution.
			a = enemyConfig.SFillA; // Use the alpha value from the enemy configuration for the enemy's fill color.

			// Set the validColor flag to true initially for the generated color, and then perform checks to see if it should be set to false and regenerated.
			validColor = true;

			// Check the generated color against luminance thresholds and reserved colors before accepting it as valid.
			float luminance = 0.299f * r + 0.587f * g + 0.114f * b; // Calculate the luminance of the generated color using the standard formula for perceived brightness.
			if (luminance < 40.0f || luminance > 220.0f) { // If the luminance of the generated color is too low (dark) or too high (bright)
				validColor = false; // Then the color is not valid, set the validColor flag to false to generate a new color.
				continue; // Continue back to the start of the loop to generate a new color without checking against reserved colors.
			} // If the color passes the luminance check, it is then checked against the reserved colors.

			for (const auto& resCol : reservedColors) { // Iterate through the reserved colors.
				// Calculate the distance squared between the generated color and the reserved color in RGB space to determine how close they are.
				int distSq = (r - resCol.r) * (r - resCol.r) + (g - resCol.g) * (g - resCol.g) + (b - resCol.b) * (b - resCol.b);
				if (distSq < 10000) { // If the distance squared is less than the threshold, the color is too close to a reserved color.
					validColor = false; // Set the validColor flag to false to generate a new color.
					break; // Break out of the reserved colors loop, back to the start of the while loop, to generate a new color without checking against the rest of the reserved colors.
				}
			}
		}
		fillColor = sf::Color(r, g, b, a); // Create an SFML Color object for the enemy's fill color using the generated RGBA values.
	}
	sf::Color outlineColor(enemyConfig.SOutR, enemyConfig.SOutG, enemyConfig.SOutB, enemyConfig.SOutA); // Create an SFML Color object for the enemy's outline color based on the config file values.

	// Create a uniform real distribution for the enemy's lifespan based on the minimum and maximum lifespan values defined in the enemy configuration.
	std::uniform_real_distribution<float> distLifespan(enemyConfig.SLifeMin, enemyConfig.SLifeMax);
	// Generate a random lifespan in seconds for the enemy using the defined distribution based on the enemy configuration values.
	float lifespan = distLifespan(m_rng);

	// Create uniform real distributions for the enemy's speed based on the minimum and maximum speed values defined in the enemy configuration.
	std::uniform_real_distribution<float> distSpeed(enemyConfig.SSpeedMin, enemyConfig.SSpeedMax);
	float speed = distSpeed(m_rng); // Generate a random speed scalar for the enemy using the defined distribution based on the enemy configuration values.

	// Create uniform real distributions for the enemy's initial position within the window bounds.
	std::uniform_real_distribution<float> distPosX(totalSize, m_window.getSize().x - totalSize);
	float posX = distPosX(m_rng); // Generate a random X position for the enemy within the window bounds, considering the total size of the enemy to prevent spawning partially outside the window.
	std::uniform_real_distribution<float> distPosY(totalSize, m_window.getSize().y - totalSize);
	float posY = distPosY(m_rng); // Generate a random Y position for the enemy within the window bounds, considering the total size of the enemy to prevent spawning partially outside the window.
	Vec2f enemyPos(posX, posY); // Create a Vec2f object for the generated enemy position to use in distance calculations.

	Vec2f playerPos = m_player->get<CTransform>().position; // Get the player's current position from the player entity's CTransform component to check against the generated enemy position.
	// Use the distanceSq function from the Vec2f class to calculate the squared distance between the generated enemy position and the player's position.
	while (enemyPos.distanceSq(playerPos) < 10000.0f) { // If the squared distance is less than the threshold, which means the enemy is spawning too close to the player,
		posX = distPosX(m_rng); // Regenerate the X position for the enemy.
		posY = distPosY(m_rng); // Regenerate the Y position for the enemy.
		enemyPos = Vec2f(posX, posY); // Update the enemy position object with the new generated position for the next distance check.
		continue; // Continue back to the start of the while loop to check the new generated position against the player's position again.
	}

	// Create a uniform real distribution for the enemy's movement direction in radians, from 0 to 2*PI.
	std::uniform_real_distribution<float> distDirection(0, 6.28318f); // 0 to 2*PI radians
	float direction = distDirection(m_rng); // Generate a random direction for the enemy to move in using the defined distribution (to be multiplied by the speed scalar to get the velocity vector).

	float velX = std::cos(direction) * speed; // Calculate the X component of the enemy's velocity based on the random direction and speed scalar.
	float velY = std::sin(direction) * speed; // Calculate the Y component of the enemy's velocity based on the random direction and speed scalar.

	if (speed != 0.0f) { // Check if the configured speed for the enemy is not zero and if it's not ensure it does not spawn stationary.
		while (velX == 0.0f && velY == 0.0f) { // If the generated velocity vector is (0,0), which would mean the enemy is stationary, regenerate the X and Y velocity components.
			direction = distDirection(m_rng); // Generate a new random direction for the enemy to move in.
			velX = std::cos(direction) * speed; // Recalculate the X component of the enemy's velocity based on the new random direction and speed scalar.
			velY = std::sin(direction) * speed; // Recalculate the Y component of the enemy's velocity based on the new random direction and speed scalar.
		}
	}

	float facingAngle = 0.0f; // Initialize the facing angle for the enemy's CTransform component to 0 degrees. This can be updated in the enemy logic system.
	if (enemyConfig.enemyType == "ShooterSt") { // If the enemy type is a shooter straight, set the facing angle to the desired shooting direction.
		facingAngle = 270.0f; // 270 degrees means the enemy will face stright-up.
	}
	else if (enemyConfig.enemyType == "ShooterRd") { // If the enemy type is a shooter random, set the initial facing angle to a random direction. It will be updated in the enemy logic system.
		facingAngle = distDirection(m_rng) * 180.0f / 3.14159265f; // Calculate a random facing angle in degrees based on the random direction converted from radians to degrees by multiplying by 180/PI.
	}
	else if (enemyConfig.enemyType == "ShooterSn") { // If the enemy type is a shooter sniper, set the initial facing angle towards the player. It will be updated in the enemy logic system.
		// Calculate the angle from the enemy to the player using atan2 and convert it from radians to degrees.
		float angleToPlayer = std::atan2(playerPos.y - posY, playerPos.x - posX) * 180.0f / 3.14159265f;
		facingAngle = angleToPlayer; // Set the facing angle to the calculated angle towards the player.
	}
	else { // For other enemy types, set the initial facing angle based on the random movement direction.
		facingAngle = direction * 180.0f / 3.14159265f; // Set the facing angle based on the random movement direction converted from radians to degrees.
	}

		// Create a new entity with the tag "enemy" using the EntityManager's createEntity function, which returns a shared pointer to the newly created enemy entity.
		auto enemy = m_entities.createEntity("enemy");

		// Add a CSubtype component to the enemy entity with the subtype value defined in the enemy configuration.
		enemy->add<CSubtype>(enemyConfig.enemyType);
		// Add a CTransform component to the enemy entity with the initial position, velocity, and angles based on the generated values and enemy configuration.
		enemy->add<CTransform>(Vec2f(posX, posY), Vec2f(velX, velY), facingAngle, facingAngle);
		// Add a CShape component to the enemy entity with the properties defined in the selected enemy configuration, using the generated values for radius, vertices, and colors.
		enemy->add<CShape>(outlineThickness, radius, vertices, fillColor, outlineColor);
		// Add a CCollision component to the enemy entity with the collision radius defined as the radius plus outline thickness for better collision detection.
		enemy->add<CCollision>(totalSize);
		// Add a CLifespan component to the enemy entity with the generated lifespan value, which will determine how long the enemy exists in the game before being removed.
		enemy->add<CLifespan>(lifespan);
		// Add a CHealth component to the enemy entity with the health value defined in the enemy configuration for the selected enemy type.
		enemy->add<CHealth>(enemyConfig.SHealth);
		// Add a CDamage component to the enemy entity with the damage value defined in the enemy configuration for the selected enemy type.
		enemy->add<CDamage>(enemyConfig.SDamage);
		// Add a CScoreValue component to the enemy entity with the score value defined in the enemy configuration for the selected enemy type.
		enemy->add<CScoreValue>(enemyConfig.SVal);
}

void Engine::spawnSmallEnemies(std::shared_ptr<Entity> parentEntity) {
	
	if (parentEntity->getTag() != "enemy") return; // Ensure that the parent entity is an enemy before attempting to spawn small enemies.

	auto& pTransform = parentEntity->get<CTransform>(); // Get a reference to the parent entity's CTransform component to determine the starting position and velocity of the small enemies.
	auto& pShape = parentEntity->get<CShape>(); // Get a reference to the parent entity's CShape component to determine the properties of the small enemies' shapes, such as color and size.
	auto& pScore = parentEntity->get<CScoreValue>(); // Get a reference to the parent entity's CScoreValue component to determine the score value of the small enemies.
	int vertices = static_cast<int>(pShape.circle.getPointCount()); // Get the number of vertices from the parent entity to determine how many small enemies to spawn and their initial directions.
	float radius = pShape.circle.getRadius() / 2.0f; // Calculate the radius for the small enemies as half of the parent entity's radius for smaller size.
	float outlineThickness = pShape.circle.getOutlineThickness() / 1.5f; // Calculate the outline thickness for the small enemies as a fraction of the parent entity's outline thickness.
	sf::Color fill = pShape.circle.getFillColor(); // Get the fill color from the parent entity to use as the initial fill color for the small enemies.
	sf::Color outline = pShape.circle.getOutlineColor(); // Get the outline color from the parent entity to use as the initial outline color for the small enemies.
	Vec2f vel = pTransform.velocity; // Get the velocity vector from the parent entity's CTransform component to use as the base velocity.
	
	float angleStep = 360.0f / vertices; // Calculate the angle step between each small enemy based on the number of vertices, ensures they are spawned equidistant in a circle around the parent entity.

	// Shoot outwards in a circle
	for (int i = 0; i < vertices; ++i) { // Loop through the number of vertices to spawn a small enemy for each vertex.
		auto smallEnemy = m_entities.createEntity("small_enemy"); // Create a new entity with the tag "small_enemy" using the EntityManager's createEntity function.

		// Convert angle to radians for trigonometric functions
		float radians = (angleStep * i) * (3.14159f / 180.0f);
		// Calculate the velocity vector for the small enemy based on the angle and the parent entity's velocity, so they shoot outwards in a circle.
		float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y); // Calculate the speed scalar from the parent entity's velocity vector to maintain the same speed for the small enemies.
		if (speed < 6.0f) speed = 75.0f; // Ensure a minimum speed for the small enemies so they don't spawn stationary if the parent entity is slow or stationary.
		Vec2f smallVel(std::cos(radians) * speed, std::sin(radians) * speed); // Calculate the velocity vector for the small enemy based on the angle and speed.
	
		// Add a CTransform component to the small enemy entity with the initial position set to the parent entity's position, velocity based on the angle and speed, and an angle of 0 degrees.
		smallEnemy->add<CTransform>(pTransform.position, smallVel, 0.0f, 0.0f);
		// Add a CShape component to the small enemy entity with the properties defined based on the parent entity's shape, using the calculated radius for size and the same colors.
		smallEnemy->add<CShape>(outlineThickness, radius, vertices, fill, outline);
		// Add a CCollision component to the small enemy entity with the collision radius defined as the calculated radius for the small enemies.
		smallEnemy->add<CCollision>(radius);
		// Add a CLifespan component to the small enemy entity with a hardcoded lifespan value, which will determine how long the small enemy exists in the game before being removed.
		smallEnemy->add<CLifespan>(2.0f);
		// Add a CHealth component to the small enemy entity with a hardcoded health value of 1, so they are weaker than the parent enemy.
		smallEnemy->add<CHealth>(1.0f);
		// Add a CDamage component to the small enemy entity with a hardcoded damage value of 1, so they are weaker than the parent enemy.
		smallEnemy->add<CDamage>(1.0f);
		// Add a CScoreValue component to the small enemy entity with the score value defined as the parent entity's score value divided by the number of vertices.
		smallEnemy->add<CScoreValue>(pScore.scoreValue / vertices);
	}
}

void Engine::spawnProjectile(std::shared_ptr<Entity> sourceEntity, const Vec2f& targetPos) {
	if (m_projectileConfigs.empty()) return; // Check if there are any projectile configurations loaded before attempting to spawn a projectile. If the vector is empty, return early to avoid errors.
	auto& sourceTransform = sourceEntity->get<CTransform>(); // Get a reference to the source entity's CTransform component to determine the starting position of the projectile.
	auto& sourceShape = sourceEntity->get<CShape>(); // Get a reference to the source entity's CShape component to determine the properties of the projectile's shape.
	std::string tag = sourceEntity->getTag(); // Get the tag of the source entity to determine which projectile configuration to use based on the source entity type (e.g., player or enemy).

	if (tag == "player") { // If the source entity is the player, check the cooldown timer for the player's firing rate before allowing a new projectile to be spawned.
		if (m_gameTime - m_lastPlayerFireTime < 0.5f) return; // If the time since the last player fire is less than 0.5 seconds, return early to enforce the cooldown and prevent spawning a new projectile.
		m_lastPlayerFireTime = m_gameTime; // Update the last player fire time to the current game time when a new projectile is spawned to start the cooldown timer.
	}

	// Select the appropriate projectile configuration based on the source entity's tag.
	int projectileType = (tag == "player") ? 0 : 1; // For example, if the source entity is the player, use the first projectile configuration; if it's an enemy, use the second configuration.
	auto& projectileConfig = m_projectileConfigs[projectileType]; // Get a reference to the selected projectile configuration for easier access to its properties when spawning the projectile entity.

	Vec2f direction; // Declare a variable to hold the direction vector for the projectile's movement.
	Vec2f spawnPos = sourceTransform.position; // Initialize the spawn position of the projectile to the source entity's position, which is offset to the source vertex.

	if (tag == "player") { // If the source entity is the player, calculate the direction vector from the player to the mouse position for the projectile to travel towards.
		direction = targetPos - sourceTransform.position; // Calculate the direction vector from the player's position to the target position (mouse position).
		direction.normalize(); // Normalize the direction vector to get a unit vector for consistent projectile speed regardless of distance to the target.

		// Offset the spawn position to be the vertex of the player's shape that is being "followed" by the mouse.
		float offset = sourceShape.circle.getRadius() + projectileConfig.SCRadius; // Calculate the offset distance from the player's center to the edge of the shape where the projectile should spawn.
		spawnPos = sourceTransform.position + direction * offset; // Set the spawn position to be at the edge of the player's shape in the direction of the target position.
	}
	else { // If the source entity is an enemy, calculate the direction vector based on the enemy's subtype.
		std::string subtype = sourceEntity->get<CSubtype>().subtype; // Get the subtype of the enemy from its CSubtype component.
		if (subtype == "ShooterSn") { // If the enemy subtype is a shooter sniper,
			// Aim at the player
			if (m_player) { // Check if the player entity exists before trying to access its position.
				// Set the direction vector to point from the enemy's position to the player's position for the projectile to travel towards the player.
				direction = m_player->get<CTransform>().position - sourceTransform.position;
			}
			// Reduce the speed of the sniper projectile to make it easier to dodge, since it is aimed directly at the player.
			projectileConfig.SSpeedMax *= 0.75f; // Reduce the maximum speed of the projectile by multiplying it by a factor less than 1.
		}
		else if (subtype == "ShooterSt") { // If the enemy subtype is a shooter straight,
			// Shoot in a straight line based on the enemy's facing angle.
			float angle = sourceTransform.facingAngle * 3.14159f / 180.0f; // Convert the facing angle from degrees to radians.
			direction = Vec2f(std::cos(angle), std::sin(angle)); // Set the direction vector based on the facing angle of the enemy.
		}
		else if (subtype == "ShooterRd") { // If the enemy subtype is a shooter random,
			float angle = sourceTransform.facingAngle * 3.14159f / 180.0f; // then convert the facing angle (which is randomly set at spawn and updated in enemy logic) from degrees to radians.
			direction = Vec2f(std::cos(angle), std::sin(angle)); // Set the direction vector based on the facing angle of the enemy.
		}
		if (direction.x == 0.0f && direction.y == 0.0f) { // Check if the generated direction vector is (0,0), which would mean the projectile has no direction.
			direction = Vec2f(0.0f, -1.0f); // If the direction is (0,0), default to shooting upwards.
		}
		direction.normalize(); // Normalize the direction vector to get a unit vector for consistent projectile speed regardless of distance to the target.

		// Offset the spawn position to be one of the vertices of the enemy's shape based on its facing angle.
		float offset = sourceShape.circle.getRadius() + projectileConfig.SCRadius; // Calculate the offset distance from the enemy's center to the edge of the shape where the projectile should spawn.
		spawnPos = sourceTransform.position + direction * offset; // Set the spawn position to be at the edge of the enemy's shape in the direction the enemy is facing.
	}
	auto projectile = m_entities.createEntity("projectile"); // Create a new entity with the tag "projectile" using the EntityManager's createEntity function.
	// Calculate the angle of the projectile based on the direction vector for setting the projectile's rotation, converting from radians to degrees.
	float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159f; // Convert the direction vector to an angle in degrees for the projectile's rotation.

	// Add a CSubtype component to the projectile entity with the subtype value defined in the projectile configuration for the selected projectile type.
	projectile->add<CSubtype>(projectileConfig.projectileType);
	// Add a CTransform component to the projectile entity with the calculated spawn position and angle, velocity based on its direction and speed from the projectile configuration.
	projectile->add<CTransform>(spawnPos, direction * projectileConfig.SSpeedMax, angle, angle);
	// Add a CShape component to the projectile entity with the properties defined in the selected projectile configuration.
	projectile->add<CShape>(
		projectileConfig.SOThickness,
		projectileConfig.SCRadius,
		projectileConfig.SVerticesMin,
		sf::Color(projectileConfig.SFillR, projectileConfig.SFillG, projectileConfig.SFillB, projectileConfig.SFillA),
		sf::Color(projectileConfig.SOutR, projectileConfig.SOutG, projectileConfig.SOutB, projectileConfig.SOutA)
	);
	// Add a CCollision component to the projectile entity with the collision radius defined in the projectile configuration for the selected projectile type.
	projectile->add<CCollision>(projectileConfig.SCRadius);
	// Add a CLifespan component to the projectile entity with the lifespan value defined in the projectile configuration for the selected projectile type.
	projectile->add<CLifespan>(projectileConfig.SLifeMax);
	// Add a CDamage component to the projectile entity with the damage value defined in the projectile configuration for the selected projectile type.
	projectile->add<CDamage>(projectileConfig.SDamage);
}

void Engine::spawnSpecialAbility(std::shared_ptr<Entity> sourceEntity, int abilityType) {
	m_currentSpecialAbility = abilityType; // Set the current special ability type to the specified ability type.
	if (!sourceEntity || !sourceEntity->isActive()) return; // Check if the source entity exists and is active before trying to access its components to spawn a special ability.
	if (m_specialAbilityCooldown > 0.0f) return; // Check if the special ability cooldown timer is greater than 0 before allowing a new special ability to be spawned.
	if (m_specialAbilityDuration > 0.0f) return; // Check if the special ability duration timer is greater than 0 before allowing a new special ability to be spawned.
	if (m_specialAbilityDuration <= 0.0f) m_specialAbilityActive = false; // If the special ability duration timer is less than or equal to 0, set the special ability active flag to false.

	auto& sourceTransform = sourceEntity->get<CTransform>(); // Get a reference to the source entity's CTransform component to determine the starting position of the special ability effects.

	// Special Ability 1: Ring of Bullets - Shoot projectiles in 8 directions (every 45 degrees)
	if (abilityType == 1) { // Check if the ability type corresponds to the Ring of Fire special ability.
		for (int i = 0; i < 8; ++i) { // Loop through 8 iterations to spawn a projectile in each of the 8 directions (every 45 degrees) around the source entity.
			float radians = (i * 45.0f) * (3.14159f / 180.0f); // Calculate the angle in radians for the current direction by multiplying the iteration index by 45 degrees and converting to radians.
			Vec2f targetPos( // Calculate the target position for the projectile to be spawned towards based on the angle and a fixed distance from the source entity's position.
				sourceTransform.position.x + std::cos(radians) * 100.0f, // Calculate the X component of the target position by adding the cosine of the angle multiplied by a fixed distance.
				sourceTransform.position.y + std::sin(radians) * 100.0f // Calculate the Y component of the target position by adding the sine of the angle multiplied by a fixed distance.
			);
			spawnProjectile(sourceEntity, targetPos); // Call the spawnProjectile function to spawn a projectile from the source entity towards the calculated target position for each of the iterations.
			m_lastPlayerFireTime = -10.0f; // Set the last player fire time to a negative value to bypass the firing cooldown for the projectiles spawned by the Ring of Fire special ability.
		}
		m_specialAbilityActive = true; // Set the special ability active flag to true to indicate that a special ability is currently active.
		m_specialAbilityCooldown = 10.0f; // Set the cooldown timer for the special ability to 10 seconds after using it.
		m_specialAbilityDuration = 0.0f; // Reset the duration timer for the special ability since this ability is an instant effect.
	}
	// Special Ability 2: Razor Shape - Set player invincible for 3 seconds and increase speed by 50%.
	else if (abilityType == 2) { // Check if the ability type corresponds to the Razor Shape special ability.
		m_specialAbilityActive = true; // Set the special ability active flag to true to indicate that a special ability is currently active.
		m_specialAbilityCooldown = 10.0f; // Set the cooldown timer for the special ability to 10 seconds after using it.
		m_specialAbilityDuration = 5.0f; // Set the duration timer for the special ability to 5 seconds.
	}
}

void Engine::sMovement() {
	// Player Input Translation into Velocity
	if (m_player && m_player->isActive()) { // Check if the player entity exists and is active before trying to access its components to update its velocity based on input.
		auto& input = m_player->get<CInput>(); // Get a reference to the player entity's CInput component to read the current input state for movement.
		auto& transform = m_player->get<CTransform>(); // Get a reference to the player entity's CTransform component to update its velocity based on the input state.

		Vec2f velocity(0.0f, 0.0f); // Initialize a velocity vector to (0,0) that will be updated based on the input state for movement in the four cardinal directions.
		if (input.up)    velocity.y -= 1.0f; // If the up input is active, decrease the Y component of the velocity to move upwards.
		if (input.down)  velocity.y += 1.0f; // If the down input is active, increase the Y component of the velocity to move downwards.
		if (input.left)  velocity.x -= 1.0f; // If the left input is active, decrease the X component of the velocity to move left.
		if (input.right) velocity.x += 1.0f; // If the right input is active, increase the X component of the velocity to move right.

		// Normalize to prevent faster diagonal movement
		if (velocity.x != 0.0f || velocity.y != 0.0f) { // Check if the velocity vector is not (0,0) to avoid normalizing a zero vector, which would cause issues.
			velocity.normalize(); // Normalize the velocity vector to ensure consistent movement speed in all directions, including diagonals, by converting it to a unit vector.
			float speed = m_playerConfig.SSpeed; // Get the base speed scalar for the player from the player configuration to use for calculating the final velocity.
			transform.velocity = velocity * speed; // Set the player's velocity in the CTransform component to be the normalized velocity vector multiplied by the speed scalar.
		}
		else { // If the velocity vector is (0,0),
			transform.velocity = Vec2f(0.0f, 0.0f); // then set the player's velocity to (0,0) to ensure the player stops moving when there is no input.
		}
		// Check if a special ability is active and if the current special ability is the Razor Shape ability, if it is then increase the player's velocity by 50% by multiplying it by 1.5.
		if (m_specialAbilityActive && m_currentSpecialAbility == 2) transform.velocity *= 1.5f;
	}
	// Apply velocity to position for all entities
	for (auto entity : m_entities.getEntities()) { // Loop through all entities in the EntityManager to apply their velocity to their position for movement.
		if (entity->has<CTransform>()) { // Check if the entity has a CTransform component before trying to access it to apply movement.
			auto& trans = entity->get<CTransform>(); // Get a reference to the entity's CTransform component to update its position based on its velocity.
			// Update the entity's position by adding its velocity multiplied by the delta time in seconds to ensure frame rate independent movement.
			trans.position += trans.velocity * m_deltaTimeSeconds;
		}
	}
}

void Engine::sCollision() {
	// detect and handle collisions between entities with collision components, such as player-enemy, player-projectile, player-wall, enemy-wall, enemy-projectile, enemy-enemy collisions.
	float winWidth = static_cast<float>(m_window.getSize().x); // Get the width of the game window as a float to use for collision checks against the window bounds.
	float winHeight = static_cast<float>(m_window.getSize().y); // Get the height of the game window as a float to use for collision checks against the window bounds.

	// Player vs Wall Bounds
	if (m_player && m_player->isActive()) { // Check if the player entity exists and is active before trying to access its components.
		auto& playerTransform = m_player->get<CTransform>(); // Get a reference to the player entity's CTransform component to check its position for collisions against the window bounds.
		auto& playerCol = m_player->get<CCollision>(); // Get a reference to the player entity's CCollision component to access its collision radius for collisions against the window bounds.
		// Check each side of the window bounds against the player's position and collision radius to prevent the player from moving outside the window.
		// If the player's position minus its collision radius is less than 0 on the X axis set the player's position to be equal to its collision radius.
		if (playerTransform.position.x - playerCol.radius < 0) playerTransform.position.x = playerCol.radius;
		// If the player's position plus its collision radius is greater than the window width on the X axis, set the player's position to be equal to the window width minus its collision radius.
		if (playerTransform.position.x + playerCol.radius > winWidth) playerTransform.position.x = winWidth - playerCol.radius;
		// If the player's position minus its collision radius is less than 0 on the Y axis, set the player's position to be equal to its collision radius.
		if (playerTransform.position.y - playerCol.radius < 0) playerTransform.position.y = playerCol.radius;
		// If the player's position plus its collision radius is greater than the window height on the Y axis, set the player's position to be equal to the window height minus its collision radius.
		if (playerTransform.position.y + playerCol.radius > winHeight) playerTransform.position.y = winHeight - playerCol.radius;
	}

	// Enemy & Small Enemy vs Wall
	// Define a lambda function to check for collisions between enemies of a given tag and the window bounds.
	auto checkWallCollision = [&](const std::string& tag) { // Lambda function that takes an enemy tag as a parameter to check for collisions against the window bounds for all enemies with that tag.
		for (auto& enemy : m_entities.getEntitiesByTag(tag)) { // Loop through all entities with the specified tag from the lambda function parameter.
			auto& enemyTransform = enemy->get<CTransform>(); // Get a reference to the enemy entity's CTransform component to check its position and velocity for collisions against the window bounds.
			auto& enemyCol = enemy->get<CCollision>(); // Get a reference to the enemy entity's CCollision component to access its collision radius for collisions against the window bounds.
			// Check each side of the window bounds against the enemy's position and collision radius to create a bounce effect when colliding with the window edges.
			// If the enemy's position minus its collision radius is less than 0 on the X axis, set the enemy's position to be equal to its collision radius and reverse its X velocity.
			if (enemyTransform.position.x - enemyCol.radius < 0) { enemyTransform.position.x = enemyCol.radius; enemyTransform.velocity.x *= -1.0f; }
			// If the enemy's position plus its collision radius is greater than the window width on the X axis, set the enemy's position and reverse its X velocity.
			if (enemyTransform.position.x + enemyCol.radius > winWidth) { enemyTransform.position.x = winWidth - enemyCol.radius; enemyTransform.velocity.x *= -1.0f; }
			// If the enemy's position minus its collision radius is less than 0 on the Y axis, set the enemy's position to be equal to its collision radius and reverse its Y velocity.
			if (enemyTransform.position.y - enemyCol.radius < 0) { enemyTransform.position.y = enemyCol.radius; enemyTransform.velocity.y *= -1.0f; }
			// If the enemy's position plus its collision radius is greater than the window height on the Y axis, set the enemy's position and reverse its Y velocity.
			if (enemyTransform.position.y + enemyCol.radius > winHeight) { enemyTransform.position.y = winHeight - enemyCol.radius; enemyTransform.velocity.y *= -1.0f; }
		}
		};
	checkWallCollision("enemy"); // Check for collisions between enemies with the tag "enemy" and the window bounds to create a bounce effect for regular enemies.
	checkWallCollision("small_enemy"); // Check for collisions between enemies with the tag "small_enemy" and the window bounds to create a bounce effect for small enemies.

	// Projectiles vs Wall
	// Loop through all entities with the tag "projectile" and check if their position plus or minus their collision radius is outside the window bounds on either axis.
	for (auto& proj : m_entities.getEntitiesByTag("projectile")) { // Loop through all entities with the tag "projectile".
		auto& projTransform = proj->get<CTransform>(); // Get a reference to the projectile entity's CTransform component to check its position for collisions against the window bounds.
		auto& projCol = proj->get<CCollision>(); // Get a reference to the projectile entity's CCollision component to access its collision radius for collisions against the window bounds.
		// If the projectile's position minus its collision radius is less than 0,
		// or if the projectile's position plus its collision radius is greater than the window width on the X axis,
		if (projTransform.position.x - projCol.radius < 0 ||
			projTransform.position.x + projCol.radius > winWidth ||
			projTransform.position.y - projCol.radius < 0 ||
			projTransform.position.y + projCol.radius > winHeight) {
			proj->destroy(); // Then destroy the projectile entity since it has collided with the window bounds and should no longer exist in the game.
		}
	}

	// Projectile vs Enemies
	for (auto& proj : m_entities.getEntitiesByTag("projectile")) { // Loop through all entities with the tag "projectile".
		if (!proj->isActive()) continue; // Check if the projectile entity is active before trying to access its components to check for collisions. If it's not active, skip to the next projectile.
		if (proj->get<CSubtype>().subtype != "PPlayer") continue; // If the projectile's subtype is not "PPlayer", skip it.
		// Define a lambda function to check for collisions between the projectile and enemies of a given tag.
		auto checkPProjectileHit = [&](const std::string& tag, bool isSmall) { // Lambda function that takes an enemy tag and a boolean indicating whether the enemies are small.
			for (auto& enemy : m_entities.getEntitiesByTag(tag)) { // Loop through all entities with the tag specified in the lambda function parameter.
				if (!enemy->isActive() || !proj->isActive()) continue; // Check if the enemy entity and projectile entity are both active before trying to access their components.

				auto& projTransform = proj->get<CTransform>(); // Get a reference to the projectile's CTransform component to access its position for collision checking.
				auto& enemyTransform = enemy->get<CTransform>(); // Get a reference to the enemy's CTransform component to access its position for collision checking.
				auto& projCol = proj->get<CCollision>(); // Get a reference to the projectile's CCollision component to access its collision radius for collision checking.
				auto& enemyCol = enemy->get<CCollision>(); // Get a reference to the enemy's CCollision component to access its collision radius for collision checking.
				// Calculate the distance squared between the projectile and enemy positions to check for a collision.
				float distSq = projTransform.position.distanceSq(enemyTransform.position);
				// Calculate the sum of the projectile's collision radius and the enemy's collision radius to determine the threshold for a collision.
				float radSum = projCol.radius + enemyCol.radius;
				if (distSq < (radSum * radSum)) { // If the distance squared is less than the sum of the radii squared, it means the projectile and enemy are colliding.
					proj->destroy(); // Destroy the projectile entity since it has hit an enemy and should no longer exist in the game.

					// Apply damage to the enemy.
					// Decrease the enemy's current health by the damage value of the projectile from its CDamage component.
					enemy->get<CHealth>().currentHealth -= proj->get<CDamage>().damageValue;

					if (enemy->get<CHealth>().currentHealth <= 0.0f) { // Check if the enemy's health has dropped to 0 or below,
						m_currentScore += enemy->get<CScoreValue>().scoreValue; // If it is, increase the player's score by the score value of the enemy from its CScoreValue component.
						enemy->destroy(); // And destroy the enemy entity.
						if (!isSmall) spawnSmallEnemies(enemy); // And if the enemy is not a small enemy, spawn small enemies from it using the spawnSmallEnemies function.
					}
					break; // Projectile is dead, stop checking enemies for it
				}
			}
			};
		checkPProjectileHit("enemy", false); // Check for collisions between player projectiles and enemies, passing false for isSmall since these are regular enemies.
		checkPProjectileHit("small_enemy", true); // Check for collisions between player projectiles and small enemies, passing true for isSmall since these are.
	}

	// Player vs Enemies
	if (m_player && m_player->isActive()) { // Check if the player entity exists and is active before trying to access its components.
		auto& playerTransform = m_player->get<CTransform>(); // Get a reference to the player entity's CTransform component to access its position for collision checking.
		auto& playerCol = m_player->get<CCollision>(); // Get a reference to the player entity's CCollision component to access its collision radius for collision checking.
		auto& playerHealth = m_player->get<CHealth>(); // Get a reference to the player entity's CHealth component to access its health for collision checking.
		// Define a lambda function to check for collisions between the player and enemies of a given tag, and apply damage to the player if a collision occurs.
		auto checkPlayerHit = [&](const std::string& tag, bool isSmall) { // Lambda function that takes an enemy tag and a boolean indicating whether the enemies are small.
			for (auto& enemy : m_entities.getEntitiesByTag(tag)) { // Loop through all entities with the tag specified in the lambda function parameter.
				if (!enemy->isActive()) continue; // Check if the enemy entity is active before trying to access its components. If it's not active, skip to the next enemy.

				auto& enemyTransform = enemy->get<CTransform>(); // Get a reference to the enemy's CTransform component to access its position for collision checking.
				auto& enemyCol = enemy->get<CCollision>(); // Get a reference to the enemy's CCollision component to access its collision radius for collision checking.

				float distSq = playerTransform.position.distanceSq(enemyTransform.position); // Calculate the distance squared between the player and enemy positions to check for a collision.
				float radSum = playerCol.radius + enemyCol.radius; // Calculate the sum of the player's collision radius and the enemy's collision radius to determine the threshold for a collision.
				if (distSq < (radSum * radSum)) { // If the distance squared is less than the sum of the radii squared, it means the player and enemy are colliding.
					m_currentScore += enemy->get<CScoreValue>().scoreValue; // Increase the player's score by the score value of the enemy from its CScoreValue component.
					enemy->destroy(); // Destroy the enemy entity upon collision with the player.

					// Initialize a boolean variable to track whether the player is currently invincible based on whether a specific special ability is active.
					bool isInvincible = m_specialAbilityActive && m_currentSpecialAbility == 2;
					if (!isInvincible) {
						// Apply damage to the player
						playerHealth.currentHealth -= enemy->get<CDamage>().damageValue;
						// Check for Player Death
						if (playerHealth.currentHealth <= 0.0f) { // If the player's health has dropped to 0 or below,
							setGameOver(true); // then set the game over state which will prompt the player to restart or exit and will save high score if applicable.
							return; // Exit the lambda function early since the player is dead and we don't need to check for more collisions.
						}
					}
					else { // Player is invincible,
						continue; // Skip damage
					}
				}
			}
			};
		checkPlayerHit("enemy", false); // Check for collisions between the player and regular enemies, passing false for isSmall since these are regular enemies
		checkPlayerHit("small_enemy", true); // Check for collisions between the player and small enemies, passing true for isSmall since these are small enemies
	}

	// Player vs Projectiles
	for (auto& proj : m_entities.getEntitiesByTag("projectile")) { // Loop through all entities with the tag "projectile".
		if (!proj->isActive()) continue; // Check if the projectile entity is active before trying to access its components to check for collisions. If it's not active, skip to the next projectile.
		if (proj->get<CSubtype>().subtype == "PPlayer") { // If the projectile's subtype is "PPlayer", which indicates it's a player projectile,
			continue; // Then skip the collision checks for this projectile since player projectiles should not collide with the player.
		}
		else { // If the projectile is not a player projectile,
			// Check for collisions between enemy projectiles and the player, and apply damage to the player if a collision occurs.
			if (m_player && m_player->isActive()) { // Check if the player entity exists and is active before trying to access its components.
				auto& playerTransform = m_player->get<CTransform>(); // Get a reference to the player entity's CTransform component to access its position for collision checking.
				auto& playerCol = m_player->get<CCollision>(); // Get a reference to the player entity's CCollision component to access its collision radius for collision checking.
				auto& playerHealth = m_player->get<CHealth>(); // Get a reference to the player entity's CHealth component to access its health for collision checking.
				auto& projTransform = proj->get<CTransform>(); // Get a reference to the projectile's CTransform component to access its position for collision checking.
				auto& projCol = proj->get<CCollision>(); // Get a reference to the projectile's CCollision component to access its collision radius for collision checking.
				float distSq = playerTransform.position.distanceSq(projTransform.position); // Calculate the distance squared between the player and projectile positions to check for a collision.
				float radSum = playerCol.radius + projCol.radius; // Calculate the sum of the player's collision radius and the projectile's collision radius to determine the threshold for a collision.
				if (distSq < (radSum * radSum)) { // If the distance squared is less than the sum of the radii squared, it means the player and projectile are colliding.
					proj->destroy(); // Destroy the projectile entity since it has hit the player and should no longer exist in the game.
					// Initialize a boolean variable to track whether the player is currently invincible based on whether a specific special ability is active.
					bool isInvincible = m_specialAbilityActive && m_currentSpecialAbility == 2;
					if (!isInvincible) {
						// Apply damage to the player
						playerHealth.currentHealth -= proj->get<CDamage>().damageValue;
						// Check for Player Death
						if (playerHealth.currentHealth <= 0.0f) { // If the player's health has dropped to 0 or below,
							setGameOver(true); // then set the game over state which will prompt the player to restart or exit and will save high score if applicable.
							return; // Exit the lambda function early since the player is dead and we don't need to check for more collisions.
						}
					}
					else { // Player is invincible,
						continue; // Skip damage
					}
				}
			}
		}
	}
}

void Engine::sLifespan() {
	for (auto& entity : m_entities.getEntities()) { // Loop through all entities in the EntityManager to check for those that have a CLifespan component and update their lifespan accordingly.
		if (entity->has<CLifespan>()) { // Check if the current entity has a CLifespan component, 
			auto& life = entity->get<CLifespan>(); // If it does, get a reference to the CLifespan component to access and modify the remaining lifespan of the entity.
			life.remainingLifespan -= m_deltaTimeSeconds; // Decrease the remaining lifespan of the entity by the delta time in seconds to ensure frame rate independent lifespan reduction.

			if (life.remainingLifespan <= 0) { // Check if the remaining lifespan of the entity has reached 0 or below.
				entity->destroy(); // If the lifespan has expired, call the destroy function on the entity to mark it for removal from the game.
			}
			else if (entity->has<CShape>()) { // If the entity still has remaining lifespan and has a CShape component,
				auto& shape = entity->get<CShape>().circle; // Then get a reference to the shape from the CShape component to modify its color based on the remaining lifespan.

				// Extract raw colors
				sf::Color fill = shape.getFillColor(); // Get the current fill color of the shape to modify its alpha value based on the remaining lifespan for a fading effect.
				sf::Color out = shape.getOutlineColor(); // Get the current outline color of the shape to modify its alpha value based on the remaining lifespan for a fading effect.

				// Calculate the alpha ratio based on the remaining lifespan compared to the total lifespan to determine how much the color should fade.
				float alphaRatio = life.remainingLifespan / life.totalLifespan;
				// Update the alpha value of the fill color based on the calculated alpha ratio to create a fading effect as the entity's lifespan decreases.
				fill.a = static_cast<sf::Uint8>(255.0f * alphaRatio); // static_cast is used to convert the resulting float value of the alpha ratio multiplied by 255 to an unsigned 8-bit integer.
				// Update the alpha value of the outline color based on the calculated alpha ratio to create a fading effect as the entity's lifespan decreases.
				out.a = static_cast<sf::Uint8>(255.0f * alphaRatio); // static_cast is used to convert the resulting float value of the alpha ratio multiplied by 255 to an unsigned 8-bit integer.
				shape.setFillColor(fill); // Set the modified fill color back to the shape to apply the fading effect visually in the game.
				shape.setOutlineColor(out); // Set the modified outline color back to the shape to apply the fading effect visually in the game.
			}
		}
	}
}

void Engine::sEnemyLogic() {
	// Enemy spawn rate logic.
	if (!m_firstEnemySpawned) { // Check if the first enemy has not been spawned yet to handle the initial spawn logic separately from the regular spawn logic.
		if (m_gameTime >= 3.0f) { // If the current game time has reached or exceeded 3 seconds, which is the defined time for the first enemy spawn,
			spawnEnemy(); // then call the spawnEnemy function to create the first enemy entity.
			m_firstEnemySpawned = true; // Set the flag to true to indicate that the first enemy has been spawned.
			m_lastEnemySpawnTime = m_gameTime; // Set the last enemy spawn time to the current game time after spawning the first enemy to initialize the spawn timer for subsequent enemy spawns.
			m_spawnInterval = m_enemyConfigs[0].SSpawnMax; // Set the initial spawn interval to the maximum spawn interval defined in the enemy configuration for the first enemy type.
		}
	}
	// Regular enemy spawn logic based on the spawn timer and interval, which controls how frequently new enemies are spawned as the game progresses.
	else if (m_gameTime - m_lastEnemySpawnTime >= m_spawnInterval) { // If it is time to spawn a new enemy based on the spawn timer,
		spawnEnemy(); // then call the spawnEnemy function to create a new enemy entity based on the defined enemy configurations and random generation logic.
		// and decrease spawn interval over time (up to SSpawnMin) to make the game more challenging as it progresses. Max and Min Spawn intervals are defined in the config file.
		if (m_spawnInterval > m_enemyConfigs[0].SSpawnMin) { // Check if the current spawn interval is greater than the minimum spawn interval defined in the enemy configuration.
			m_spawnInterval -= 0.1f; // If it is, decrease the spawn interval by a small amount (0.1 seconds) to gradually increase the spawn rate of enemies over time.
		}
		// Ensure spawn interval does not go below the defined minimum spawn interval to prevent it from becoming too fast and unmanageable for the player.
		if (m_spawnInterval < m_enemyConfigs[0].SSpawnMin) { // Check if the spawn interval has decreased below the minimum spawn interval defined in the enemy configuration.
			m_spawnInterval = m_enemyConfigs[0].SSpawnMin; // If it has, set the spawn interval back to the minimum spawn interval to maintain a manageable spawn rate for the player.
		}
		m_lastEnemySpawnTime = m_gameTime; // Update the last enemy spawn time to the current game time after spawning an enemy to reset the spawn timer for the next enemy spawn.
	}

	// Enemy fire rate logic.
	for (auto& enemy : m_entities.getEntitiesByTag("enemy")) { // Loop through all entities with the tag "enemy" to check for shooter enemies and handle their firing logic based on their subtype.
		if (!enemy->isActive()) continue; // Check if the enemy entity is active before trying to access its components to handle firing logic. If it's not active, skip to the next enemy.
		auto& subtype = enemy->get<CSubtype>().subtype; // Get a reference to the enemy's CSubtype component to access its subtype string for determining its behavior.
		auto& enemyTransform = enemy->get<CTransform>(); // Get a reference to the enemy's CTransform component to access and modify its facing angle.

		// ShooterSn: update facing angle every frame to continuously track the player's current position.
		if (subtype == "ShooterSn" && m_player && m_player->isActive()) { // Check if the enemy's subtype is "ShooterSn" and if the player entity exists and is active, if so
			Vec2f toPlayer = m_player->get<CTransform>().position - enemyTransform.position; // Calculate the vector from the enemy to the player's current position.
			// Update the enemy's facing angle to point towards the player by calculating the arctangent of the vector to the player and converting it from radians to degrees.
			enemyTransform.facingAngle = std::atan2(toPlayer.y, toPlayer.x) * 180.0f / 3.14159265f;
		}

		if (subtype == "ShooterSn" || subtype == "ShooterSt" || subtype == "ShooterRd") {
			float fireInterval = 2.0f; // All shooter types fire every 2 seconds.
			// Use the entity's ID to spread fire times so multiple shooters don't all fire on the same frame.
			float idOffset = std::fmod(static_cast<float>(enemy->getId()) * 0.7f, fireInterval); // Offset each shooter's fire cycle by a unique amount based on its ID to desynchronize their shots.
			float currPhase = std::fmod(m_gameTime + idOffset, fireInterval); // Current position within this shooter's 2-second fire cycle.
			float prevPhase = std::fmod(m_gameTime - m_deltaTimeSeconds + idOffset, fireInterval); // Position within the cycle at the start of this frame.

			// ShooterRd pre-rotation: randomize facing angle 0.5 seconds before the shot fires so the visual turn precedes the bullet.
			if (subtype == "ShooterRd") {
				float preRotateThreshold = fireInterval - 0.5f; // 1.5 seconds into the cycle = 0.5 seconds before the 2-second fire point.
				bool crossedPreRotate = (prevPhase < preRotateThreshold && currPhase >= preRotateThreshold); // True only on the single frame that crosses the 1.5-second threshold.
				if (crossedPreRotate) {
					std::uniform_real_distribution<float> distAngle(0.0f, 360.0f); // Full 360-degree range for random direction.
					enemyTransform.facingAngle = distAngle(m_rng); // Set a new random facing angle; spawnProjectile reads this when the shot fires 0.5 seconds later.
				}
			}

			// Fire detection: the phase wraps from near-2.0 back to near-0.0 when a new 2-second cycle begins.
			bool crossedFire = (currPhase < prevPhase); // True only on the single frame when the phase resets, indicating it's time to fire.
			if (crossedFire) {
				spawnProjectile(enemy, Vec2f(0.0f, 0.0f)); // Fire a projectile; direction is derived from facingAngle inside spawnProjectile.
			}
		}
	}
}

void Engine::sGUI() { // Renders the in-game debug UI using ImGui, providing live system toggles, entity inspection, and spawner controls.
	ImGui::Begin("Geometry Wars Plus Debugger"); // Begin the ImGui debug window.

	// --- Pause Control ---
	ImGui::Checkbox("Pause Game", &m_paused); // Checkbox to toggle the paused state directly from the debug UI, mirroring the P key behavior.

	// --- System Toggles ---
	ImGui::Separator(); // Visual divider between pause control and system toggles.
	ImGui::Text("Systems (` to toggle this window)"); // Section label; reminds the developer that backtick hides/shows this window.
	ImGui::Checkbox("Movement", &m_sMovementEnabled);   // Enables or disables sMovement for isolating movement behavior during debugging.
	ImGui::SameLine();
	ImGui::Checkbox("Collision", &m_sCollisionEnabled);  // Enables or disables sCollision for isolating collision behavior during debugging.
	ImGui::SameLine();
	ImGui::Checkbox("Lifespan", &m_sLifespanEnabled);   // Enables or disables sLifespan for freezing entity lifetimes during debugging.
	ImGui::SameLine();
	ImGui::Checkbox("Enemy Logic", &m_sEnemyLogicEnabled); // Enables or disables sEnemyLogic for pausing enemy spawning and shooter fire independently.

	// --- Spawn Interval Slider ---
	ImGui::Separator();
	float minInterval = m_enemyConfigs.empty() ? 0.5f : m_enemyConfigs[0].SSpawnMin; // Minimum spawn interval from config or default if no configs loaded.
	float maxInterval = m_enemyConfigs.empty() ? 10.0f : m_enemyConfigs[0].SSpawnMax; // Maximum spawn interval from config or default if no configs loaded.
	ImGui::SliderFloat("Spawn Interval (s)", &m_spawnInterval, minInterval, maxInterval); // Slider to adjust the current enemy spawn interval in real time within the config-defined range.

	// --- Entity Tracker ---
	ImGui::Separator();
	ImGui::Text("Total Entities: %zu", m_entities.getEntities().size()); // Display the total active entity count.

	if (ImGui::CollapsingHeader("Active Entities")) { // Collapsible section for the full entity list.
		for (auto& e : m_entities.getEntities()) {
			if (!e->isActive()) continue; // Skip inactive entities pending removal.
			auto& pos = e->get<CTransform>().position;
			ImGui::Text("ID: %zu | Tag: %s | Pos: (%.0f, %.0f)", e->getId(), e->getTag().c_str(), pos.x, pos.y); // Display each entity's ID, tag, and current position.
			ImGui::SameLine();
			std::string btnLabel = "Destroy##" + std::to_string(e->getId()); // Unique button label using the entity's ID; "##" is ImGui's convention for hidden unique widget IDs.
			if (ImGui::Button(btnLabel.c_str())) e->destroy(); // Mark the entity for removal if its destroy button is clicked.
		}
	}

	// --- Manual Spawner ---
	ImGui::Separator();
	if (ImGui::Button("Spawn Enemy Manually")) spawnEnemy(); // Immediately spawn an enemy outside the normal spawn interval for quick testing.

	// --- Delete Save Data Button ---
	ImGui::Separator();
	if (ImGui::Button("Delete Save Data")) { // Button to wipe all persistent save data and reset records to zero, useful for testing fresh sessions during development.
		std::remove("AppData/SavedGame.dat"); // Delete the save file from disk so no records carry over to the next session.
		m_highScore = 0; // Reset the in-memory high score to zero immediately so the HUD reflects the deletion right away.
		m_bestTimeSurvived = 0.0f; // Reset the in-memory best time to zero immediately so the HUD reflects the deletion right away.
	}

	ImGui::End(); // Finalize the ImGui window layout for this frame.
}

void Engine::sRender() {
	m_window.clear(); // Clear the window at the beginning of the render function to prepare for drawing the current frame's content.

	// Loop through all entities in the EntityManager to check for those that have a CShape and CTransform component, for rendering shapes correctly.
	for (auto entity : m_entities.getEntities()) { // Loop through all entities in the EntityManager to render those that have a CShape component.
		// Check if the entity has both a CShape and CTransform component, which are required for rendering the shape at the correct position and rotation.
		if (entity->has<CShape>() && entity->has<CTransform>()) {
			auto& transComp = entity->get<CTransform>(); // Get a reference to the entity's CTransform component to access its position and angle for rendering.
			auto& shapeComp = entity->get<CShape>(); // Get a reference to the entity's CShape component to access its shape properties for rendering.
			
			// If the entity is the player, set its visual angle to point towards the current mouse position to create a dynamic aiming effect where the player's shape rotates to face the mouse cursor.
			if (entity->getTag() == "player") { // If the entity is the player, 
				auto mousePos = sf::Mouse::getPosition(m_window); // then get the current mouse position relative to the game window,
				Vec2f diff( // and calculate the vector from the player's position to the mouse position:
					// static_cast is used since sf::Mouse::getPosition returns an sf::Vector2i (with int components) and we need to convert it to float for the Vec2f used in our CTransform.
					static_cast<float>(mousePos.x) - transComp.position.x, // Calculate the X component of the difference vector from the player's position to the mouse position,
					static_cast<float>(mousePos.y) - transComp.position.y // and calculate the Y component of the difference vector from the player's position to the mouse position.
				);
				// Use the atan2 function to calculate the angle in radians from the difference vector, then convert it to degrees,
				transComp.visualAngle = std::atan2(diff.y, diff.x) * (180.0f / 3.14159f) + 90.0f; // and add 90 degrees to adjust for the default orientation of the player's shape.
			}

			// If the entity is a shooter enemy subtype, set its visual angle to match its facing angle so they are "aiming" in the direction they will fire.
			if (entity->has<CSubtype>() && (entity->get<CSubtype>().subtype == "ShooterSn" || entity->get<CSubtype>().subtype == "ShooterSt" || entity->get<CSubtype>().subtype == "ShooterRd")) {
				transComp.visualAngle = transComp.facingAngle + 90.0f; // Set the visual angle of shooter enemies to match their facing angle plus 90 degrees to adjust for the default orientation.
			}
			// If the entity is not the player, or any of the shooter enemy subtypes, apply a slow rotation effect to make them visually distinct and dynamic in the game.
			else {
				// Increment the angle of the entity's CTransform component by n degrees multiplied by delta time to ensure frame rate independence.
				transComp.visualAngle += 90.0f * m_deltaTimeSeconds; // Set n to 90 degrees per second for a noticeable but not too fast rotation effect.
			}
			// Set the position and rotation of the shape based on the entity's CTransform component before drawing it to the window.
			shapeComp.circle.setPosition(transComp.position.x, transComp.position.y);
			// The angle is already being updated in the CTransform component, so we just need to set the shape's rotation to match it for proper rendering.
			shapeComp.circle.setRotation(transComp.visualAngle);
			// Draw the shape to the window using the SFML draw function, which will render the shape with its current properties (position, rotation, color, etc.) to the game window.
			m_window.draw(shapeComp.circle);

			//Logic for creating an indicator tip on the Shape to indicate where the shape is effectively "aiming".
			if (entity->getTag() == "player") { // If the entity is the player,
				float radius = shapeComp.circle.getRadius(); // then get the radius of the player's shape to determine how far from the center the indicator tip should be placed,
				// and convert the player's visual angle from degrees to radians for use in trigonometric calculations,
				float angleRad = (transComp.visualAngle - 90.0f) * (3.14159f / 180.0f); 
				// then calculate the X coordinate of the indicator tip by adding the cosine of the angle multiplied by the radius to the player's X position,
				float tipX = transComp.position.x + std::cos(angleRad) * radius;
				// and calculate the Y coordinate of the indicator tip by adding the sine of the angle multiplied by the radius to the player's Y position.
				float tipY = transComp.position.y + std::sin(angleRad) * radius;

				// Create a small circle shape to serve as the indicator tip, set its properties, and draw it to the window on top of the player's shape.
				sf::CircleShape tipIndicator(radius * 0.2f, 3); // Create a small circle shape for the indicator tip, with a radius that is a fraction of the player's shape.
				tipIndicator.setFillColor(sf::Color::Red); // Set the color of the indicator tip for visibility.
				tipIndicator.setOrigin(tipIndicator.getRadius(), tipIndicator.getRadius()); // Set the origin of the indicator tip to its center for accurate positioning.)
				tipIndicator.setPosition(tipX, tipY); // Set the position of the indicator tip to the calculated coordinates.
				tipIndicator.setRotation(transComp.visualAngle); // Rotate the indicator tip to match the visual angle of the shooter enemy for a consistent aiming indicator.
				m_window.draw(tipIndicator); // Draw the indicator tip to the window.
			}
			else if (entity->has<CSubtype>() && (entity->get<CSubtype>().subtype == "ShooterSn" || entity->get<CSubtype>().subtype == "ShooterSt" || entity->get<CSubtype>().subtype == "ShooterRd")) { // If the entity is a shooter enemy subtype,
				float radius = shapeComp.circle.getRadius(); // then get the radius of the enemy's shape to determine how far from the center the indicator tip should be placed,
				// and convert the enemy's visual angle from degrees to radians for use in trigonometric calculations,
				float angleRad = (transComp.visualAngle - 90.0f) * (3.14159f / 180.0f);
				// then calculate the X coordinate of the indicator tip by adding the cosine of the angle multiplied by the radius to the enemy's X position
				float tipX = transComp.position.x + std::cos(angleRad) * radius;
				// and calculate the Y coordinate of the indicator tip by adding the sine of the angle multiplied by the radius to the enemy's Y position.
				float tipY = transComp.position.y + std::sin(angleRad) * radius;

				// Create a small circle shape to serve as the indicator tip, set its properties, and draw it to the window on top of the player's shape.
				sf::CircleShape tipIndicator(radius * 0.2f, 3); // Create a small circle shape for the indicator tip, with a radius that is a fraction of the player's shape.
				tipIndicator.setFillColor(sf::Color::Cyan); // Set the color of the indicator tip for visibility.
				tipIndicator.setOrigin(tipIndicator.getRadius(), tipIndicator.getRadius()); // Set the origin of the indicator tip to its center for accurate positioning.)
				tipIndicator.setPosition(tipX, tipY); // Set the position of the indicator tip to the calculated coordinates.
				tipIndicator.setRotation(transComp.visualAngle); // Rotate the indicator tip to match the visual angle of the shooter enemy for a consistent aiming indicator.
				m_window.draw(tipIndicator); // Draw the indicator tip to the window.
			}
		}
	}

	// Render the player HUD with current score, high score, time survived, highest time survived, health, and special ability information at the top of the window.
	std::stringstream specialAbilityStatus; // Create a string variable to hold the status of the player's current special ability for display in the HUD.
	if (m_currentSpecialAbility == 2 && m_specialAbilityActive) { // If SA2 is active,
		// then set the special ability status string to indicate that SA2 is active and display the remaining duration of the special ability in seconds.
		specialAbilityStatus << "Razor Shape Active: " << std::fixed << std::setprecision(2) << m_specialAbilityDuration << "s remaining!";
	}
	else if (m_currentSpecialAbility == 2 && m_specialAbilityCooldown > 0.0f) { // If SA2 is selected but is on cooldown,
		// then set the special ability status string to indicate that SA2 is on cooldown and display the remaining cooldown duration in seconds.
		specialAbilityStatus << "Razor Shape Cooldown: " << std::fixed << std::setprecision(2) << m_specialAbilityCooldown << "s remaining!";
	}
	else if (m_currentSpecialAbility == 2 && m_specialAbilityCooldown <= 0.0f && !m_specialAbilityActive) { // If SA2 is selected but is not active and not on cooldown,
		// then set the special ability status string to indicate that SA2 is ready to use.
		specialAbilityStatus << "Special Ability: Razor Shape Ready!     (RMB to Activate | Tab to Switch)";
	}
	else if (m_currentSpecialAbility == 1 && m_specialAbilityCooldown <= 0.0f && !m_specialAbilityActive) { // If SA1 is selected and is not on cooldown,
		// then set the special ability status string to indicate that SA1 is ready to use.
		specialAbilityStatus << "Special Ability: Ring of Bullets Ready!     (RMB to Activate | Tab to Switch)";
	}
	else if (m_currentSpecialAbility == 1 && m_specialAbilityCooldown > 0.0f) { // If SA1 is selected but is on cooldown,
		// then set the special ability status string to indicate that SA1 is on cooldown and display the remaining cooldown duration in seconds.
		specialAbilityStatus << "Ring of Bullets Cooldown: " << std::fixed << std::setprecision(2) << m_specialAbilityCooldown << "s remaining!";
	}

	m_text.setOrigin(0, 0); // Set the origin of the text to the top-left corner for easier positioning at the top of the window.
	m_text.setPosition(10, 10); // Set the position of the text to be slightly offset from the top-left corner of the window for better visibility.
	m_text.setCharacterSize(18); // Set the character size of the text to 18 for better readability in the HUD.
	m_text.setFillColor(sf::Color::White); // Set the fill color of the text to white for better contrast against the dark background of the game window.
	
	std::stringstream hudStream; // Create a stringstream to build the HUD text with multiple lines and variables for better readability and maintainability.
	hudStream << "Score: " << m_currentScore << "  High Score: " << m_highScore << "\n" <<
		"Time: " << std::fixed << std::setprecision(3) << m_timeSurvived << "s  Best Time: " << std::fixed << std::setprecision(3) << m_bestTimeSurvived << "s\n" <<
		"Health: " << m_player->get<CHealth>().currentHealth << " / " << m_player->get<CHealth>().totalHealth << "\n" <<
		specialAbilityStatus.str() << "\n" <<
		"Press 'P' to Pause";
	m_text.setString(hudStream.str());
	m_window.draw(m_text); // Draw the HUD text to the window to display the player's current score, high score, time survived, best time survived, and health.
	
	float winW = static_cast<float>(m_window.getSize().x);
	float winH = static_cast<float>(m_window.getSize().y);

	// Lambda to draw a single centered text line at a vertical offset from the screen center.
	auto drawCenteredText = [&](const std::string& str, unsigned int size, float yOffset, sf::Color color) {
		m_text.setString(str);
		m_text.setCharacterSize(size);
		m_text.setFillColor(color);
		sf::FloatRect bounds = m_text.getLocalBounds();
		m_text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f); // Center the origin on the text's visual midpoint for accurate screen-center placement.
		m_text.setPosition(winW / 2.0f, winH / 2.0f + yOffset);
		m_window.draw(m_text);
		};

	if (m_gameOver) {
		sf::RectangleShape overlay(sf::Vector2f(winW, winH));
		overlay.setFillColor(sf::Color(0, 0, 0, 160)); // Semi-transparent black to darken the game world behind the overlay.
		m_window.draw(overlay);
		drawCenteredText("GAME OVER", 72, -90.0f, sf::Color::Red);
		drawCenteredText("Score: " + std::to_string(m_currentScore), 32, -15.0f, sf::Color::White);
		drawCenteredText("High Score: " + std::to_string(m_highScore), 28, 25.0f, sf::Color::Yellow);

		std::stringstream timeStream; // Create a stringstream to format the time survived and best time with fixed decimal places for better readability and maintainability.
		timeStream << "Time: " << std::fixed << std::setprecision(3) << m_timeSurvived << "s  Best Time: " <<
			std::fixed << std::setprecision(3) << m_bestTimeSurvived << "s";

		drawCenteredText(timeStream.str(), 24, 62.0f, sf::Color::Cyan);
		drawCenteredText("Press Esc to Save & Quit | Or Any OTHER Key to Restart", 22, 105.0f, sf::Color::White);
	}

	else if (m_paused) {
		sf::RectangleShape overlay(sf::Vector2f(winW, winH));
		overlay.setFillColor(sf::Color(0, 0, 0, 120)); // Lighter overlay to visually distinguish pause from game over.
		m_window.draw(overlay);
		drawCenteredText("PAUSED", 60, -20.0f, sf::Color::White);
		drawCenteredText("R to Restart | P to Resume | Esc to Save & Quit", 24, 40.0f, sf::Color::Yellow);
	}

	m_text.setOrigin(0.0f, 0.0f); // Reset text origin after overlay rendering so next frame's HUD text positions correctly.dw
	ImGui::SFML::Render(m_window); // Render the ImGui UI elements to the window after drawing the game entities and HUD, so that the UI appears on top of the game content.
	m_window.display(); // Display the rendered frame on the window to update the visuals for the current frame and show the player the latest game state.
}

void Engine::sInput() {
	sf::Event event; // Create an SFML event object to store the events polled from the window for processing player input and interactions.
	while (m_window.pollEvent(event)) { // Poll events from the window in a loop to process all pending events for the current frame.
		ImGui::SFML::ProcessEvent(event); // Pass the polled event to ImGui's event processing function to allow ImGui to handle input in the ImGui window.

		if (event.type == sf::Event::Closed) { // If the event type is "Closed", which occurs when the player clicks the window's close button,
			saveGameData(); // then call the saveGameData function to save the player's game data.
			m_running = false; // and set the running flag to false to exit the main game loop and close the game window properly.
		}

		// Let ImGui "eat" inputs if a window is active.
		if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard) { // Check if ImGui wants to capture mouse or keyboard input,
			continue; // Skip processing the event in the game if ImGui wants to capture it.
		}

		if (event.type == sf::Event::KeyPressed) { // If the event type is "KeyPressed", which occurs when the player presses a key on the keyboard,
			if (m_gameOver || m_paused) { // If the game is currently in a game over state or paused state,
				if (event.key.code == sf::Keyboard::Escape) { // and if the Escape key is pressed, 
					saveGameData(); // then call the saveGameData function to save the player's game data,
					m_running = false; // and set the running flag to false to exit the main game loop and close the game window properly.
				}
			}

			// If the game is over and at least 2 seconds have passed since the game over state was entered, and if the pressed key is not Escape,
			if (m_gameOver && m_gameOverTime >= 2.0f && event.key.code != sf::Keyboard::Escape) {
				restartGame(); // then restart the game, resetting all state and spawning a fresh player entity for the new session.
			}

			if (event.key.code == sf::Keyboard::Grave) { // Backtick (`) toggles the ImGui debug UI window on and off.
				m_sGuiEnabled = !m_sGuiEnabled;
			}
			
			if (!m_gameOver) { // If the game is not over,
				if (event.key.code == sf::Keyboard::P) { // and if the P key is pressed, 
					setPaused(!m_paused); // then toggle the paused state by calling the setPaused function with the opposite of the current paused state to either pause or unpause the game.
				}
			}
			if (m_paused) { // If the game is currently paused
				if (event.key.code == sf::Keyboard::R) { // and the R key is pressed,
					restartGame(); // then call the restartGame function to reset the game state and start a new game.
				}
			}

			if (m_player && m_player->isActive()) { // If the player entity exists and is active, then process movement input for the player
				auto& input = m_player->get<CInput>(); // Get a reference to the player's CInput component to update its input state.
				if (event.key.code == sf::Keyboard::W) input.up = true; // If the W key is pressed, set the up input state to true in the player's CInput component.
				if (event.key.code == sf::Keyboard::S) input.down = true; // If the S key is pressed, set the down input state to true in the player's CInput component.
				if (event.key.code == sf::Keyboard::A) input.left = true; // If the A key is pressed, set the left input state to true in the player's CInput component.
				if (event.key.code == sf::Keyboard::D) input.right = true; // If the D key is pressed, set the right input state to true in the player's CInput component.
				if (event.key.code == sf::Keyboard::Tab) { // If the Tab key is pressed, toggle between the two special abilities by
					// setting the current special ability to 2 if it was previously 1, or back to 1 if it was previously 2, via a ternary operator.
					m_currentSpecialAbility = (m_currentSpecialAbility == 1) ? 2 : 1;
				}
			}
		}

		if (event.type == sf::Event::KeyReleased) { // If the event type is "KeyReleased", which occurs when the player releases a key on the keyboard,
			if (m_player && m_player->isActive()) { // If the player entity exists and is active, then process movement input for the player
				auto& input = m_player->get<CInput>(); // Get a reference to the player's CInput component to update its input state.
				if (event.key.code == sf::Keyboard::W) input.up = false; // If the W key is released, set the up input state to false in the player's CInput component.
				if (event.key.code == sf::Keyboard::S) input.down = false; // If the S key is released, set the down input state to false in the player's CInput component.
				if (event.key.code == sf::Keyboard::A) input.left = false; // If the A key is released, set the left input state to false in the player's CInput component.
				if (event.key.code == sf::Keyboard::D) input.right = false; // If the D key is released, set the right input state to false in the player's CInput component.
			}
		}

		if (event.type == sf::Event::MouseButtonPressed) { // If the event type is "MouseButtonPressed", which occurs when the player presses a mouse button,
			if (m_player && m_player->isActive() && !m_paused && !m_gameOver) { // If the player entity exists, is active, and the game is not paused, or over, then process mouse input.
				Vec2f mousePos((float)event.mouseButton.x, (float)event.mouseButton.y); // Get the position of the mouse click from the event data.
				// If the left mouse button is pressed, call the spawnProjectile function with the player entity as the source and the mouse position as the target.
				if (event.mouseButton.button == sf::Mouse::Left) {
					spawnProjectile(m_player, mousePos); // Call the spawnProjectile function with the player entity as the source and the mouse position as the target.
				}
				// If the right mouse button is pressed, call the spawnSpecialAbility function with the player entity as the source.
				else if (event.mouseButton.button == sf::Mouse::Right) {
					spawnSpecialAbility(m_player, m_currentSpecialAbility); // Call the spawnSpecialAbility function with the player entity as the source and the current special ability index.
				}
			}
		}
	}
}