#include "Engine.h" // Include the header file that contains the declaration of the Engine class and its variables and functions.
#include <fstream> // Include the fstream library for file input and output operations. Required for loading the configuration file (config.txt) in the Engine constructor.
#include <iostream> // Include the iostream library for input and output operations. Required for printing error messages to the console.
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
				int width, height, framerateLimit, fullscreen; // Declare integer variables to hold the window configuration values read from the file.
				std::string windowTitle; // Declare a string variable to hold the window title read from the file.
				fin >> width >> height >> framerateLimit >> fullscreen; // Read in window configuration values from the file, including width, height, framerate limit, fullscreen flag.
				std::getline(fin, windowTitle); // Read the rest of the line for the window title, allowing for spaces in the title.
				if (fullscreen) { // Check if the fullscreen flag is set in the config file. If it is,
					m_window.create(sf::VideoMode(width, height), windowTitle, sf::Style::Fullscreen); // Then create a fullscreen window using the SFML RenderWindow's create function.
				}
				else {
					m_window.create(sf::VideoMode(width, height), windowTitle); // Create a windowed mode window using the SFML RenderWindow's create function.
				}
				m_window.setFramerateLimit(framerateLimit); // Set the window's framerate limit based on the config file value.
				
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
	else {
		std::cerr << "Could not load config file: " << configPath << " - Terminating.\n"; // Print an error message to the console if the configuration file could not be opened.
		return; // Terminate the initialization process if the config file cannot be loaded, as the game cannot run without its configuration data.
	}
	if (!m_enemyConfigs.empty()) { // Check if any enemy configurations were loaded from the config file. If there are enemy configurations, 
		m_spawnInterval = m_enemyConfigs[0].SSpawnMax; // Then set the initial spawn interval to the maximum spawn interval of the first enemy configuration loaded from the config file.
	
	}
	spawnPlayer(); // Call the helper function to spawn the player entity at the start of the game.
}

void Engine::run() { // Implementation of the run function, which contains the main game loop that updates and renders the game until it is closed.
	while (m_running) { // Main game loop that continues running until the m_running flag is set to false, which can happen when the player closes the window or when the game is otherwise terminated.
		m_entities.update(); // Add entities from the "Waiting Room." 
		ImGui::SFML::Update(m_window, m_deltaClock.restart()); // Update the ImGui-SFML frame, passing in the SFML window and the time elapsed since the last frame (delta time).
		
		sInput(); // Call the input handling system to process player input and update the game state accordingly.
		if (!m_paused && !m_gameOver) { // Only update the game state if the game is not currently paused or over.
			sEnemyLogic(); // Call the enemy spawning system to check if it's time to spawn new enemies based on the current frame number and the spawn intervals.
			sMovement(); // Call the movement system to update the positions of all entities based on their velocity and the elapsed time since the last frame.
			sCollision(); // Call the collision system to check for and handle collisions between entities.
			sLifespan(); // Call the lifespan system to check for entities that have exceeded their lifespan and remove them from the game.
			m_currentFrame++; // Increment the current frame number to keep track of how many frames have elapsed since the start of the game.
		}
		sGUI(); // Call the GUI system.
		sRender(); // Call the rendering system to draw all entities and the GUI to the window.
	}
	ImGui::SFML::Shutdown(); // Shutdown the ImGui-SFML integration when the game loop ends.
}

void Engine::setPaused(bool paused) { // Implementation of the setPaused function, which sets the paused state of the game.
	m_paused = paused; // Set the m_paused flag to the provided value to indicate whether the game is currently paused or not.
	// Display a "Paused" message in the middle of the window when the game is paused, and hide it when the game is unpaused.
}

void Engine::setGameOver(bool gameOver) { // Implementation of the setGameOver function, which sets the game over state of the game.
	m_gameOver = gameOver; // Set the m_gameOver flag to the provided value to indicate whether the game is currently over or not.
	// Display a "Game Over" message in the middle of the window when the game is over, prompting the user to exit or restart and hide it when the game is restarted.
	// Save the player's score to the high score if it is greater than the current high score, and display the high score on the game over screen.
	// Save the high score to a file so it persists between game sessions, and load it at the start of the game to display in the window during gameplay and on the game over screen.
	// Implement a restart function that resets the game state, including the player's score, position, health, all non-player entities, and game over flag.
}

void Engine::spawnPlayer() {
	// Create a new entity with the tag "Player" using the EntityManager's createEntity function, which returns a shared pointer to the newly created entity.
	auto player = m_entities.createEntity("player");

	float centerX = m_window.getSize().x / 2.0f; // Calculate the center X coordinate of the window by dividing the window's width by 2, and store it.
	float centerY = m_window.getSize().y / 2.0f; // Calculate the center Y coordinate of the window by dividing the window's height by 2, and store it.
	
	// Add a CTransform component to the player entity with the initial position set to the center of the window, zero velocity, and an angle of 0 degrees.
	player->add<CTransform>(Vec2f(centerX, centerY), Vec2f(0.0f, 0.0f), 0.0f);
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

	m_player = player; // Store a reference to the player entity in the Engine class for easy access when updating the player's state based on input and collisions during the game loop.
}

void Engine::spawnEnemy() {
	if (m_enemyConfigs.empty()) return; // Check if there are any enemy configurations loaded before attempting to spawn an enemy. If the vector is empty, return early to avoid errors.

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

	std::uniform_int_distribution<int> distLifespan(enemyConfig.SLifeMin, enemyConfig.SLifeMax);
	int lifespan = distLifespan(m_rng); // Generate a random lifespan for the enemy using the defined distribution based on the enemy configuration values.

	std::uniform_real_distribution<float> distSpeed(enemyConfig.SSpeedMin, enemyConfig.SSpeedMax);
	float speed = distSpeed(m_rng); // Generate a random speed scalar for the enemy using the defined distribution based on the enemy configuration values.

	std::uniform_real_distribution<float> distPosX(totalSize, m_window.getSize().x - totalSize);
	float posX = distPosX(m_rng); // Generate a random X position for the enemy within the window bounds, considering the total size of the enemy to prevent spawning partially outside the window.
	std::uniform_real_distribution<float> distPosY(totalSize, m_window.getSize().y - totalSize);
	float posY = distPosY(m_rng); // Generate a random Y position for the enemy within the window bounds, considering the total size of the enemy to prevent spawning partially outside the window.
	// Don't allow enemies to spawn too close to the player by checking the distance from the generated position to the player's position and regenerating if it's too close.
	Vec2f enemyPos(posX, posY); // Create a Vec2f object for the generated enemy position to use in distance calculations.
	Vec2f playerPos = m_player->get<CTransform>().position; // Get the player's current position from the player entity's CTransform component to check against the generated enemy position.
	// Use the distanceSq function from the Vec2f class to calculate the squared distance between the generated enemy position and the player's position.
	while (enemyPos.distanceSq(playerPos) < 10000.0f) { // If the squared distance is less than the threshold, which means the enemy is spawning too close to the player,
		posX = distPosX(m_rng); // Regenerate the X position for the enemy.
		posY = distPosY(m_rng); // Regenerate the Y position for the enemy.
		enemyPos = Vec2f(posX, posY); // Update the enemy position object with the new generated position for the next distance check.
		continue; // Continue back to the start of the while loop to check the new generated position against the player's position again.
	}

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

	// Create a new entity with the tag "enemy" using the EntityManager's createEntity function, which returns a shared pointer to the newly created enemy entity.
	auto enemy = m_entities.createEntity("enemy");

	// Add a CSubtype component to the enemy entity with the subtype value defined in the enemy configuration.
	enemy->add<CSubtype>(enemyConfig.enemyType);
	// Add a CTransform component to the enemy entity with the initial position, velocity, and angle based on the generated values and enemy configuration.
	enemy->add<CTransform>(Vec2f(posX, posY), Vec2f(velX, velY), 0.0f);
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

	m_lastEnemySpawnFrame = m_currentFrame; // Update the m_lastEnemySpawnFrame variable to the current frame number to track when the last enemy was spawned for timing future spawns.
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
		if (speed < 0.1f) speed = 0.1f; // Ensure a minimum speed for the small enemies so they don't spawn stationary if the parent entity is slow or stationary.
		Vec2f smallVel(std::cos(radians) * speed, std::sin(radians) * speed); // Calculate the velocity vector for the small enemy based on the angle and speed.
	
		// Add a CTransform component to the small enemy entity with the initial position set to the parent entity's position, velocity based on the angle and speed, and an angle of 0 degrees.
		smallEnemy->add<CTransform>(pTransform.position, smallVel, 0.0f);
		// Add a CShape component to the small enemy entity with the properties defined based on the parent entity's shape, using the calculated radius for size and the same colors.
		smallEnemy->add<CShape>(outlineThickness, radius, vertices, fill, outline);
		// Add a CCollision component to the small enemy entity with the collision radius defined as the calculated radius for the small enemies.
		smallEnemy->add<CCollision>(radius);
		// Add a CLifespan component to the small enemy entity with a hardcoded lifespan value, which will determine how long the small enemy exists in the game before being removed.
		smallEnemy->add<CLifespan>(120);
		// Add a CHealth component to the small enemy entity with a hardcoded health value of 1, so they are weaker than the parent enemy.
		smallEnemy->add<CHealth>(1.0f);
		// Add a CDamage component to the small enemy entity with a hardcoded damage value of 1, so they are weaker than the parent enemy.
		smallEnemy->add<CDamage>(1.0f);
		// Add a CScoreValue component to the small enemy entity with the score value defined as the parent entity's score value divided by the number of vertices.
		smallEnemy->add<CScoreValue>(pScore.scoreValue / vertices);
	}
}

void Engine::spawnProjectile(std::shared_ptr<Entity> sourceEntity, const Vec2f& targetPos) {
	// should not be as fast as the user can click but should have a cooldown timer of 0.5 seconds.
	
	if (m_projectileConfigs.empty()) return; // Check if there are any projectile configurations loaded before attempting to spawn a projectile. If the vector is empty, return early to avoid errors.
	auto& sourceTransform = sourceEntity->get<CTransform>(); // Get a reference to the source entity's CTransform component to determine the starting position of the projectile.
	auto& sourceShape = sourceEntity->get<CShape>(); // Get a reference to the source entity's CShape component to determine the properties of the projectile's shape.
	std::string tag = sourceEntity->getTag(); // Get the tag of the source entity to determine which projectile configuration to use based on the source entity type (e.g., player or enemy).

	// Select the appropriate projectile configuration based on the source entity's tag.
	int projectileType = (tag == "player") ? 0 : 1; // For example, if the source entity is the player, use the first projectile configuration; if it's an enemy, use the second configuration.
	auto& projectileConfig = m_projectileConfigs[projectileType]; // Get a reference to the selected projectile configuration for easier access to its properties when spawning the projectile entity.

	Vec2f direction; // Declare a variable to hold the direction vector for the projectile's movement.
	Vec2f spawnPos = sourceTransform.position; // Initialize the spawn position of the projectile to the source entity's position, which is the center of enemies and offset for the player.

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
		}
		else if (subtype == "ShooterSt") { // If the enemy subtype is a shooter straight,
			// Shoot in a straight line based on the enemy's movement direction (velocity vector).
			direction = sourceTransform.velocity; // Set the direction vector to be the same as the enemy's velocity vector for the projectile to shoot in the direction the enemy is moving.
		}
		else { // For other enemy subtypes, you can define different behaviors for the projectile's direction as needed.
			// For example, you could have them shoot in a random direction.
			std::uniform_real_distribution<float> distAngle(0, 6.28318f); // Create a uniform real distribution for generating a random angle between 0 and 2*PI radians (360 degrees).
			float angle = distAngle(m_rng); // Generate a random angle for the projectile's direction.
			direction = Vec2f(std::cos(angle), std::sin(angle)); // Set the direction vector based on the random angle.
		}
		if (direction.x == 0.0f && direction.y == 0.0f) { // Check if the generated direction vector is (0,0), which would mean the projectile has no direction.
			direction = Vec2f(0.0f, -1.0f); // If the direction is (0,0), default to shooting upwards.
		}
		direction.normalize(); // Normalize the direction vector to get a unit vector for consistent projectile speed regardless of distance to the target.
	}
	auto projectile = m_entities.createEntity("projectile"); // Create a new entity with the tag "projectile" using the EntityManager's createEntity function.
	// Calculate the angle of the projectile based on the direction vector for setting the projectile's rotation, converting from radians to degrees.
	float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159f; // Convert the direction vector to an angle in degrees for the projectile's rotation.

	// Add a CSubtype component to the projectile entity with the subtype value defined in the projectile configuration for the selected projectile type.
	projectile->add<CSubtype>(projectileConfig.projectileType);
	// Add a CTransform component to the projectile entity with the calculated spawn position and angle, velocity based on its direction and speed from the projectile configuration.
	projectile->add<CTransform>(spawnPos, direction * projectileConfig.SSpeedMax, angle);
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

void Engine::spawnSpecialAbility(std::shared_ptr<Entity> sourceEntity) {
	// should not be spamable and should have a cooldown timer of 10 seconds that is tracked and displayed in the UI.
	auto& sourceTransform = sourceEntity->get<CTransform>();

	// Special Ability 1: Ring of Fire - Shoot projectiles in 8 directions (every 45 degrees)
	for (int i = 0; i < 8; ++i) {
		float radians = (i * 45.0f) * (3.14159f / 180.0f);
		Vec2f targetPos(
			sourceTransform.position.x + std::cos(radians) * 100.0f,
			sourceTransform.position.y + std::sin(radians) * 100.0f
		);
		spawnProjectile(sourceEntity, targetPos);
	}
	// Special Ability 2: Razor Shape - Set player invincible for 3 seconds and increase speed by 50%.
	/*
	
	*/
	// Special Ability 3: Cannibals - Set enemies to damage each other on collision for 3 seconds and gain score for each enemy destroyed by another enemy during this time.
	/*
	
	*/
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
			// Calculate the frame speed based on the player's speed from the configuration and the frame rate (assuming 60 FPS) to ensure consistent movement speed regardless of frame rate.
			float frameSpeed = m_playerConfig.SSpeed / 60.0f;
			// Set the player's velocity in the CTransform component to be the normalized velocity vector multiplied by the frame speed to update the player's movement based on input.
			transform.velocity = velocity * frameSpeed;
		}
		else {
			transform.velocity = Vec2f(0.0f, 0.0f); // If there is no input, set the player's velocity to (0,0) to stop movement.
		}
	}
	// Apply velocity to position for all entities
	for (auto entity : m_entities.getEntities()) { // Loop through all entities in the EntityManager to apply their velocity to their position for movement.
		if (entity->has<CTransform>()) { // Check if the entity has a CTransform component before trying to access it to apply movement.
			auto& trans = entity->get<CTransform>(); // Get a reference to the entity's CTransform component to update its position based on its velocity.
			trans.position += trans.velocity; // Update the entity's position by adding its velocity vector to its current position to move the entity based on its velocity.
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

					// If player is using special ability 2, they are invincible and should not take damage.
					bool isInvincible = m_player->has<CSpecialAbility>() && m_player->get<CSpecialAbility>().abilityType == "2";
					if (!isInvincible) {
						// Apply damage to the player
						playerHealth.currentHealth -= enemy->get<CDamage>().damageValue;
						// Check for Player Death
						if (playerHealth.currentHealth <= 0.0f) {
							// Set game over state which will prompt the player to restart or exit and will save high score if applicable.
							// Reset player position to center
							playerTransform.position = Vec2f(winWidth / 2.0f, winHeight / 2.0f);
							// Reset player velocity
							playerTransform.velocity = Vec2f(0.0f, 0.0f);
							// Reset player health
							playerHealth.currentHealth = playerHealth.totalHealth;
							// Reset score
							m_currentScore = 0;
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
					// If player is using special ability 2, they are invincible and should not take damage.
					bool isInvincible = m_player->has<CSpecialAbility>() && m_player->get<CSpecialAbility>().abilityType == "2";
					if (!isInvincible) {
						// Apply damage to the player
						playerHealth.currentHealth -= proj->get<CDamage>().damageValue;
						// Check for Player Death
						if (playerHealth.currentHealth <= 0.0f) {
							// Set game over state which will prompt the player to restart or exit and will save high score if applicable.
							// Reset player position to center
							playerTransform.position = Vec2f(winWidth / 2.0f, winHeight / 2.0f);
							// Reset player velocity
							playerTransform.velocity = Vec2f(0.0f, 0.0f);
							// Reset player health
							playerHealth.currentHealth = playerHealth.totalHealth;
							// Reset score
							m_currentScore = 0;
						}
					}
				}
			}
		}
	}
}

void Engine::sLifespan() {
	// update the remaining lifespan of entities with a lifespan component and mark them for destruction when their lifespan expires, using the lifespan system logic.
	// when lifespan is 0 destroy the entity
	for (auto& entity : m_entities.getEntities()) { // Loop through all entities in the EntityManager to check for those that have a CLifespan component and update their lifespan accordingly.
		if (entity->has<CLifespan>()) { // Check if the current entity has a CLifespan component, 
			auto& life = entity->get<CLifespan>(); // If it does, get a reference to the CLifespan component to access and modify the remaining lifespan of the entity.
			life.remainingLifespan--; // Decrease the remaining lifespan of the entity by 1 for each frame that it exists in the game.

			if (life.remainingLifespan <= 0) { // Check if the remaining lifespan of the entity has reached 0 or below.
				entity->destroy(); // If the lifespan has expired, call the destroy function on the entity to mark it for removal from the game.
			}
			else if (entity->has<CShape>()) { // If the entity still has remaining lifespan and has a CShape component,
				auto& shape = entity->get<CShape>().circle; // Then get a reference to the shape from the CShape component to modify its color based on the remaining lifespan.

				// Extract raw colors
				sf::Color fill = shape.getFillColor(); // Get the current fill color of the shape to modify its alpha value based on the remaining lifespan for a fading effect.
				sf::Color out = shape.getOutlineColor(); // Get the current outline color of the shape to modify its alpha value based on the remaining lifespan for a fading effect.

				// Calculate the alpha ratio based on the remaining lifespan compared to the total lifespan to determine how much the color should fade.
				float alphaRatio = static_cast<float>(life.remainingLifespan) / static_cast<float>(life.totalLifespan);
				// Update the alpha value of the fill color based on the calculated alpha ratio to create a fading effect as the entity's lifespan decreases.
				fill.a = static_cast<sf::Uint8>(255.0f * alphaRatio);
				// Update the alpha value of the outline color based on the calculated alpha ratio to create a fading effect as the entity's lifespan decreases.
				out.a = static_cast<sf::Uint8>(255.0f * alphaRatio);

				shape.setFillColor(fill); // Set the modified fill color back to the shape to apply the fading effect visually in the game.
				shape.setOutlineColor(out); // Set the modified outline color back to the shape to apply the fading effect visually in the game.
			}
		}
	}
}

void Engine::sEnemyLogic() {
	if (m_enemyConfigs.empty()) return; // Check if there are any enemy configurations loaded before attempting to spawn enemies. If the vector is empty, return early to avoid errors.
	// Enemy spawn rate logic.
	// Check if the number of frames since the last enemy spawn is greater than or equal to the current spawn interval to determine if it's time to spawn a new enemy.
	if (m_currentFrame - m_lastEnemySpawnFrame >= m_spawnInterval) { // If the current frame number minus the frame number of the last enemy spawn is greater than or equal to the current spawn interval,
		spawnEnemy(); // Then call the spawnEnemy function to create a new enemy entity based on the defined enemy configurations and random generation logic.
		// And decrease spawn interval over time (up to SSpawnMin) to make the game more challenging as it progresses. Max and Min Spawn intervals are defined in the config file.
		if (m_spawnInterval > m_enemyConfigs[0].SSpawnMin) { // Check if the current spawn interval is greater than the minimum spawn interval defined in the enemy configuration.
			m_spawnInterval--; // If it is, decrease the spawn interval by 1 frame to increase the spawn rate of enemies.
		}
		// Ensure spawn interval does not go below the defined minimum spawn interval to prevent it from becoming too fast and unmanageable for the player.
		if (m_spawnInterval < m_enemyConfigs[0].SSpawnMin) { // Check if the spawn interval has decreased below the minimum spawn interval defined in the enemy configuration.
			m_spawnInterval = m_enemyConfigs[0].SSpawnMin; // If it has, set the spawn interval back to the minimum spawn interval to maintain a manageable spawn rate for the player.
		}
	}

	// Enemy fire rate logic.
	for (auto& enemy : m_entities.getEntitiesByTag("enemy")) { // Loop through all entities with the tag "enemy" to find the shooter subtypes that can spawn projectiles.
		if (!enemy->isActive()) { // If the enemy entity is not active,
			continue; // Then skip it and continue to the next one.
		}
		auto& subtype = enemy->get<CSubtype>().subtype; // Get the subtype of the enemy from its CSubtype component to determine if it is a shooter that can spawn projectiles.
		// If the enemy subtype is a shooter, and the current frame number is a multiple of n with no remainder, then it's time for that enemy to "fire" a projectile.
		if ((subtype == "ShooterSn" || subtype == "ShooterSt") && m_currentFrame % 120 == 0) { // n = 120 means the shooter will "fire" every 120 frames, which is every 2 seconds at 60 FPS.
			// Call the spawnProjectile function with the enemy entity as the source and a dummy target position.
			spawnProjectile(enemy, Vec2f(0.0f, 0.0f)); // The spawnProjectile function will determine the actual direction based on the enemy subtype.
		}
	}
}

void Engine::sGUI() {
	// render the in-game debug UI using ImGui.
	// should be able to hide and show the UI with a key press (e.g., F1) and should not interfere with player input when active.
	// should be able to disable and enable each system in the engine individually with checkboxes in the UI to help with debugging and testing.
	// should be able to display all entities and all entities by tag in the UI with the ability to delete any selected entity.
	// should be able to spawn enemies and modify the spawn interval in real-time using the UI.
	ImGui::Begin("Geometry Wars Plus Debugger"); // Begin a new ImGui window with the title "Geometry Wars Plus Debugger" to contain all the debug UI elements for the game.
	// System Toggles
	ImGui::Checkbox("Pause Game", &m_paused); // Checkbox to toggle the paused state of the game, which will affect whether the update systems run or not.

	// Entity Tracker
	ImGui::Separator(); // Separator to visually divide the previous UI section from the next UI section.
	// Display total number of entities currently in the game by getting the size of the vector of entities from the EntityManager and showing it in the UI.
	ImGui::Text("Total Entities: %zu", m_entities.getEntities().size());

	// Display all entities and their positions
	// Create a collapsing header in the UI for "Active Entities" that can be expanded or collapsed, and if it's expanded,
	if (ImGui::CollapsingHeader("Active Entities")) {
		for (auto& e : m_entities.getEntities()) { // Loop through all entities in the EntityManager to display their information in the UI.
			if (!e->isActive()) continue; // Check if the entity is active before trying to access its components. If it's not active, skip it and continue to the next one.

			auto& pos = e->get<CTransform>().position; // Get a reference to the position of the entity from its CTransform component to display it in the UI.
			// Display the entity's ID, tag, and position in the UI using ImGui::Text for each active entity in the game.
			ImGui::Text("ID: %zu | Tag: %s | Pos: (%.0f, %.0f)", e->getId(), e->getTag().c_str(), pos.x, pos.y);

			ImGui::SameLine(); // Place the UI element on the same line as the previous text.
			// Create a unique button ID for ImGui
			// Create a button with a label that includes the entity's ID to allow for destroying the entity directly from the UI.
			std::string btnLabel = "Destroy##" + std::to_string(e->getId()); // The "##" in the button label is an ImGui convention to create a unique identifier.
			if (ImGui::Button(btnLabel.c_str())) { // If the button is clicked,
				e->destroy(); // Then call the destroy function on the entity to mark it for removal from the game.
			}
		}
	}

	// Spawner Controls
	ImGui::Separator(); // Separator to visually divide the previous UI section from the next UI section.
	if (ImGui::Button("Spawn Enemy Manually")) { // Create a button in the UI labeled "Spawn Enemy Manually" that, if clicked,
		spawnEnemy(); // Then calls the spawnEnemy function to create a new enemy entity in the game immediately.
	}
	ImGui::End(); // End the ImGui window to finalize the layout and rendering of the debug UI for this frame.
}

void Engine::sRender() {
	/* Render all entities with a shape component and render the player's current score, high score, time survived, highest time survived, and health at the top of the window, 
	And game state messages in the middle of the window. */
	// Window clear, update, draw, display
	
	m_window.clear(); // Clear the window at the beginning of the render function to prepare for drawing the current frame's content.

	// Logic to make the player's shape "follow" the mouse position by updating the player's CTransform angle based on the mouse position relative to the player's position.
	if (m_player && m_player->isActive()) { // If the player entity exists and is active,
		auto mousePos = sf::Mouse::getPosition(m_window); // Get the current position of the mouse relative to the game window to determine where the player should be "aiming" or "facing" towards.
		auto& posTrans = m_player->get<CTransform>(); // Get a reference to the player's CTransform component to update its angle based on the mouse position for the "follow" behavior.
		// Calculate the difference vector from the player's position to the mouse position to determine the angle for the player to face towards the mouse pointer.
		Vec2f diff = Vec2f((float)mousePos.x, (float)mousePos.y) - posTrans.position;
		// Use the atan2 function to calculate the angle in radians from the difference vector, then convert it to degrees for the player's rotation.
		posTrans.angle = std::atan2(diff.y, diff.x) * (180.0f / 3.14159f); // Set the player's CTransform angle to the calculated angle.
	}

	// Loop through all entities in the EntityManager to check for those that have a CShape and CTransform component, for rendering shapes correctly.
	for (auto entity : m_entities.getEntities()) { // Loop through all entities in the EntityManager to render those that have a CShape component.
		// Check if the entity has both a CShape and CTransform component, which are required for rendering the shape at the correct position and rotation.
		if (entity->has<CShape>() && entity->has<CTransform>()) {
			auto& transComp = entity->get<CTransform>(); // Get a reference to the entity's CTransform component to access its position and angle for rendering.
			auto& shapeComp = entity->get<CShape>(); // Get a reference to the entity's CShape component to access its shape properties for rendering.

			// Make non-player shapes slowly rotate over time
			if (entity->getTag() == "player") { // Check if the entity's tag is not "player" to apply rotation to all other entities except the player.
				// Add a 90 degree offset to the player's angle in the rendering code below to make the shape's "point" face towards the mouse pointer,
				// since SFML shapes typically start facing to the right (0 degrees).
				transComp.angle += 90.0f; // Set the player's shape rotation to its angle plus 90 degrees every frame to make it "follow" the mouse pointer correctly.
			}
			else { // For non-player entities,
				// Increment the angle of the entity's CTransform component by n degrees each frame to create a slow rotation effect.
				transComp.angle += 1.5f; // Set n to 1.5 degrees per frame.
			}
			// Set the position and rotation of the shape based on the entity's CTransform component before drawing it to the window.
			shapeComp.circle.setPosition(transComp.position.x, transComp.position.y);
			// The angle is already being updated in the CTransform component, so we just need to set the shape's rotation to match it for proper rendering.
			shapeComp.circle.setRotation(transComp.angle);
			// Draw the shape to the window using the SFML draw function, which will render the shape with its current properties (position, rotation, color, etc.) to the game window.
			m_window.draw(shapeComp.circle);
		}
	}

	// Render the player HUD with current score, high score, time survived, highest time survived, and health at the top of the window.
	m_text.setOrigin(0, 0); // Set the origin of the text to the top-left corner for easier positioning at the top of the window.
	m_text.setPosition(10, 10); // Set the position of the text to be slightly offset from the top-left corner of the window for better visibility.
	m_text.setCharacterSize(18); // Set the character size of the text to 18 for better readability in the HUD.
	m_text.setFillColor(sf::Color::White); // Set the fill color of the text to white for better contrast against the dark background of the game window.
	m_text.setString("Score: " + std::to_string(m_currentScore) + "  High Score: " + std::to_string(m_highScore) + "\n" +
		"Time Survived: " + std::to_string(m_timeSurvived) + "  Best Time Survived: " + std::to_string(m_bestTimeSurvived) + "\n" +
		"Health: " + std::to_string(m_player->get<CHealth>().currentHealth));
	m_window.draw(m_text); // Draw the HUD text to the window to display the player's current score, high score, time survived, best time survived, and health.
	ImGui::SFML::Render(m_window); // Render the ImGui UI elements to the window after drawing the game entities and HUD, so that the UI appears on top of the game content.
	m_window.display(); // Display the rendered frame on the window to update the visuals for the current frame and show the player the latest game state.
}

void Engine::sInput() {
	// handle player input and update the player's input component accordingly.
	// handle if the mouse is on the ImGui window.
	// poll events, key presses and releases, mouse button presses and releases, mouse position, etc.. if statements
	sf::Event event; // Create an SFML event object to store the events polled from the window for processing player input and interactions.
	while (m_window.pollEvent(event)) { // Poll events from the window in a loop to process all pending events for the current frame.
		ImGui::SFML::ProcessEvent(event); // Pass the polled event to ImGui's event processing function to allow ImGui to handle input in the ImGui window.

		if (event.type == sf::Event::Closed) { // If the event type is "Closed", which occurs when the player clicks the window's close button,
			m_running = false; // Then set the running flag to false to exit the main game loop and close the game window properly.
		}

		// Let ImGui "eat" inputs if a window is active.
		if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard) { // Check if ImGui wants to capture mouse or keyboard input,
			continue; // Skip processing the event in the game if ImGui wants to capture it.
		}

		if (event.type == sf::Event::KeyPressed) { // If the event type is "KeyPressed", which occurs when the player presses a key on the keyboard,
			if (event.key.code == sf::Keyboard::Escape) m_running = false; // If the Escape key is pressed, set the running flag to false.
			if (event.key.code == sf::Keyboard::P) setPaused(!m_paused); // If the P key is pressed, toggle the paused state of the game by calling the setPaused function.

			if (m_player && m_player->isActive()) { // If the player entity exists and is active, then process movement input for the player
				auto& input = m_player->get<CInput>(); // Get a reference to the player's CInput component to update its input state.
				if (event.key.code == sf::Keyboard::W) input.up = true; // If the W key is pressed, set the up input state to true in the player's CInput component.
				if (event.key.code == sf::Keyboard::S) input.down = true; // If the S key is pressed, set the down input state to true in the player's CInput component.
				if (event.key.code == sf::Keyboard::A) input.left = true; // If the A key is pressed, set the left input state to true in the player's CInput component.
				if (event.key.code == sf::Keyboard::D) input.right = true; // If the D key is pressed, set the right input state to true in the player's CInput component.
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
			if (m_player && m_player->isActive() && !m_paused) { // If the player entity exists, is active, and the game is not paused, then process mouse input.
				Vec2f mousePos((float)event.mouseButton.x, (float)event.mouseButton.y); // Get the position of the mouse click from the event data.
				// If the left mouse button is pressed, call the spawnProjectile function with the player entity as the source and the mouse position as the target.
				if (event.mouseButton.button == sf::Mouse::Left) {
					spawnProjectile(m_player, mousePos); // Call the spawnProjectile function with the player entity as the source and the mouse position as the target.
				}
				// If the right mouse button is pressed, call the spawnSpecialAbility function with the player entity as the source.
				else if (event.mouseButton.button == sf::Mouse::Right) {
					spawnSpecialAbility(m_player);  // Call the spawnSpecialAbility function with the player entity as the source.
				}
			}
		}
	}
}