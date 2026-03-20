#include "Engine.h" // Include the header file that contains the declaration of the Engine class and its variables and functions.
#include <fstream> // Include the fstream library for file input and output operations. Required for loading the configuration file (config.txt) in the Engine constructor.
#include <iostream> // Include the iostream library for input and output operations. Required for printing error messages to the console.
#include "imgui.h" // Include the ImGui library for creating graphical user interfaces (GUI) in the game. Required for rendering the in-game debug UI.
#include "imgui-SFML.h" // Include the ImGui-SFML binding library for integrating ImGui with SFML. Required for rendering ImGui interfaces within the SFML window.
constexpr float outlineThickness = 3.0f;
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

	// Setup Default Parameters: If the config file is missing or corrupted, the engine should still provide a playable experience rather than just crashing.
	// Default Window Setup:
	m_window.create(sf::VideoMode(1280, 720), "RizzyEngine: 0.5.0: Geometry Wars Plus"); // Create the SFML RenderWindow with a specified video mode (width, height) and title.
	m_window.setFramerateLimit(60); // Set the frame rate limit for the window to 60 frames per second to ensure consistent performance and timing in the game.

	// Default ImGui Setup:
	ImGui::SFML::Init(m_window); // Initialize the ImGui-SFML binding with the created SFML window to enable rendering of ImGui interfaces within the window.
	ImGui::StyleColorsDark(); // Set the ImGui style to a dark color scheme for better visibility and aesthetics in the in-game debug UI.
	ImGui::GetStyle().ScaleAllSizes(1.5f); // Scale up all ImGui style sizes by a factor of 1.5 to make the UI elements larger and more readable.
	ImGui::GetIO().FontGlobalScale = 1.5f; // Set the global font scale for ImGui to 1.5 to make the text in the UI larger and more readable.

	// Default Player Setup:
	m_playerConfig = { // Initialize the player configuration struct with default values for the player's properties, which will be used to spawn the player entity if the config file is missing or corrupted.
		outlineThickness, // SOThickness: Default shape outline thickness for the player's shape component
		33.0f, 35.0f, // SRadius, SCRadius: Default shape and collision radius for the player's shape and collision components.
		3, // SVertices: Default shape vertices for the player's shape component (triangle).
		0, 255, 255, 255, // SFillR, SFillG, SFillB, SFillA: Default shape fill color RGBA values for the player's shape component (cyan).
		0, 255, 0, 255, // SOutR, SOutG, SOutB, SOutA,: Default shape outline color RGBA values and outline thickness for the player's shape component (green outline).
		100.0f, // SSpeed: Default speed scalar value for the player's movement.
		2 // SHealth: Default health value for the player's health component.
		// override these default values with the values loaded from the config file if they are successfully loaded to customize the player's properties.
	};
	// Config Parsing (Overrides defaults ONLY if file exists)
	std::ifstream fin(configPath);
	if (fin.is_open()) {
		std::string header;
		while (fin >> header) {
			if (header == "player") {
				fin >> m_playerConfig.SRadius >> m_playerConfig.SCRadius
					>> m_playerConfig.SVertices >> m_playerConfig.SFillR
					>> m_playerConfig.SFillG >> m_playerConfig.SFillB
					>> m_playerConfig.SFillA >> m_playerConfig.SOutR
					>> m_playerConfig.SOutG >> m_playerConfig.SOutB
					>> m_playerConfig.SOutA >> m_playerConfig.SOThickness
					>> m_playerConfig.SSpeed >> m_playerConfig.SHealth;
			}
		}
	}
	else {
		std::cerr << "Could not load config file: " << configPath << " - Using defaults.\n";
	}
	spawnPlayer(); // Call the helper function to spawn the player entity at the start of the game.
}


void Engine::run() { // Implementation of the run function, which contains the main game loop that updates and renders the game until it is closed.
	while (m_running) {
		m_entities.update(); // Add entities from the "Waiting Room." 
		ImGui::SFML::Update(m_window, m_deltaClock.restart()); 
		
		sInput();
		if (!m_paused && !m_gameOver) {
			sEnemySpawner();
			sMovement();
			sCollision();
			sLifespan();
			m_currentFrame++;
		}
		sGUI();
		sRender();
	}
	ImGui::SFML::Shutdown();
}

void Engine::setPaused(bool paused) { // Implementation of the setPaused function, which sets the paused state of the game.
	m_paused = paused; // Set the m_paused flag to the provided value to indicate whether the game is currently paused or not.
}

void Engine::setGameOver(bool gameOver) { // Implementation of the setGameOver function, which sets the game over state of the game.
	m_gameOver = gameOver; // Set the m_gameOver flag to the provided value to indicate whether the game is currently over or not.
}

void Engine::spawnPlayer() {
	// Create a new entity with the tag "Player" using the EntityManager's createEntity function, which returns a shared pointer to the newly created entity.
	auto player = m_entities.createEntity("player");
	// make one of the vericies of the player entity "follow" the mouse
	// add default components to the player entity that can be overridden by the config file values if they are successfully loaded.
	// add read in config file functionality that overwrites the deafault components.
	float centerX = m_window.getSize().x / 2.0f;
	float centerY = m_window.getSize().y / 2.0f;
	player->add<CTransform>(Vec2f(centerX, centerY), Vec2f(0.0f, 0.0f), 0.0f);
	player->add<CShape>(
		m_playerConfig.SOThickness,
		m_playerConfig.SRadius,
		m_playerConfig.SVertices,
		sf::Color(m_playerConfig.SFillR, m_playerConfig.SFillG, m_playerConfig.SFillB, m_playerConfig.SFillA),
		sf::Color(m_playerConfig.SOutR, m_playerConfig.SOutG, m_playerConfig.SOutB, m_playerConfig.SOutA)
	);
	player->add<CCollision>(m_playerConfig.SCRadius);
	player->add<CHealth>(static_cast<float>(m_playerConfig.SHealth));
	player->add<CInput>();

	m_player = player;
}

void Engine::spawnEnemy() {
	// add default components to the enemy entity that can be overridden by the config file values if they are successfully loaded.
	// should spawn randomly within the bounds of the window
	// should spawn random enemies much more often then special enemies, but special enemies should have their spawn interval incremented down so the game becomes progressively harder.
	// add read in config file functionality that overwrites the deafault components. 
	// enemies should start their specified or randomized color and fade to black or transparent based on their rememaining lifespan.
	// Update the m_lastEnemySpawnFrame variable to the current frame number to track when the last enemy was spawned for timing future spawns based on the enemy spawn rate.
	auto enemy = m_entities.createEntity("enemy");

	// Define distributions ranges
	std::uniform_real_distribution<float> distRad(16.0f, 66.0f);
	std::uniform_real_distribution<float> distPosX(50.0f, m_window.getSize().x - 50.0f);
	std::uniform_real_distribution<float> distPosY(50.0f, m_window.getSize().y - 50.0f);
	std::uniform_real_distribution<float> distVel(-2.0f, 2.0f);
	std::uniform_int_distribution<int> distColor(0, 255);
	std::uniform_int_distribution<int> distVerts(3, 8);

	float ex = distPosX(m_rng);
	float ey = distPosY(m_rng);
	float vx = distVel(m_rng);
	float vy = distVel(m_rng);
	float rad = distRad(m_rng);


	int r = distColor(m_rng);
	int g = distColor(m_rng);
	int b = distColor(m_rng);
	int verts = distVerts(m_rng);

	// Prevent an enemy from spawning perfectly stationary
	if (vx == 0 && vy == 0) {
		vx = 1.0f, vy = 1.0f;
	}

	enemy->add<CTransform>(Vec2f(ex, ey), Vec2f(vx, vy), 0.0f);
	enemy->add<CShape>(outlineThickness, rad, verts, sf::Color(r, g, b, 255), sf::Color(255, 0, 0, 255));
	enemy->add<CCollision>(rad + outlineThickness);
	enemy->add<CLifespan>(300);
	enemy->add<CHealth>(1.0f);
	enemy->add<CDamage>(1.0f);
	enemy->add<CScoreValue>(100);

	m_lastEnemySpawnFrame = m_currentFrame;
}

void Engine::spawnSmallEnemies(std::shared_ptr<Entity> parentEntity) {
	// read parent entity's properties to determine the properties of the smaller enemies to spawn
	// hardcode health as 1 and lifespan so they dont have the tank's health and exist for as long for example.
	// spawn a number of small enemies equal to the parents vertices with the same color but half the size(radius).
	// small enemies should spawn equidistant from the midpoint and travel in a straightline outward from the midpoint.
	// small enemies score value is double the parent's.
	// small enemies should start their parent color and fade to black or transparent based on their rememaining lifespan.
	if (parentEntity->getTag() != "enemy") return;

	auto& pTransform = parentEntity->get<CTransform>();
	auto& pShape = parentEntity->get<CShape>();
	auto& pScore = parentEntity->get<CScoreValue>();
	int vertices = pShape.circle.getPointCount();
	float radius = pShape.circle.getRadius() / 2.0f;
	sf::Color fill = pShape.circle.getFillColor();
	sf::Color outline = pShape.circle.getOutlineColor();
	
	float angleStep = 360.0f / vertices;

	// Shoot outwards in a circle
	for (int i = 0; i < vertices; ++i) {
		auto smallEnemy = m_entities.createEntity("small_enemy");

		// Convert angle to radians for trigonometric functions
		float radians = (angleStep * i) * (3.14159f / 180.0f);

		// Speed constant of 3.0f, easily abstracted to config later
		Vec2f vel(std::cos(radians) * 3.0f, std::sin(radians) * 3.0f);

		smallEnemy->add<CTransform>(pTransform.position, vel, 0.0f);
		smallEnemy->add<CShape>(outlineThickness, radius, vertices, fill, outline);
		smallEnemy->add<CCollision>(radius);
		smallEnemy->add<CLifespan>(120); // Hardcoded smaller enemy lifespan
		smallEnemy->add<CHealth>(1.0f);
		smallEnemy->add<CDamage>(1.0f);
		smallEnemy->add<CScoreValue>(pScore.scoreValue * 2);
	}
}

void Engine::spawnProjectile(std::shared_ptr<Entity> sourceEntity, const Vec2f& mousePos) {
	// add default components to the projectile entity that can be overridden by the config file values if they are successfully loaded
	// add read in config file functionality that overwrites the deafault components.
	// projectiles should start white and grey to black based on their rememaining lifespan.
	// projecile should start at the verticie (or just outside of it) that follows the player mouse
	auto projectile = m_entities.createEntity("projectile");
	auto& sourceTransform = sourceEntity->get<CTransform>();

	// Calculate direction towards mouse
	Vec2f diff = mousePos - sourceTransform.position;
	diff.normalize();

	// Default bullet speed scalar
	float projectileSpeed = 15.0f;
	Vec2f vel = diff * projectileSpeed;

	projectile->add<CTransform>(sourceTransform.position, vel, 0.0f);
	projectile->add<CShape>(2.0f, 10.0f, 16, sf::Color(255, 255, 255, 255), sf::Color(100, 100, 100, 255));
	projectile->add<CCollision>(10.0f);
	projectile->add<CLifespan>(60);
	projectile->add<CDamage>(1.0f);
}

void Engine::spawnSpecialAbility(std::shared_ptr<Entity> sourceEntity) {
	// add default components to the special ability entity based on the source entity's properties that can be overridden by the config file values if they are successfully loaded.
	// add read in config file functionality that overwrites the deafault components.
	// Simple 1st Special Ability: Ring of Fire (Shoots projectiles in 8 directions)
	// 2nd special ability implemented will make player invincible for 3 seconds but still destroys enemies on collision with player.
	// 3rd special ability will make enemies do damage to eachother on collision with each other for 3 seconds
	auto& sourceTransform = sourceEntity->get<CTransform>();

	for (int i = 0; i < 8; ++i) {
		float radians = (i * 45.0f) * (3.14159f / 180.0f);
		Vec2f targetPos(
			sourceTransform.position.x + std::cos(radians) * 100.0f,
			sourceTransform.position.y + std::sin(radians) * 100.0f
		);
		spawnProjectile(sourceEntity, targetPos);
	}
}

void Engine::sMovement() {
	// Player Input Translation into Velocity
	if (m_player && m_player->isActive()) {
		auto& input = m_player->get<CInput>();
		auto& transform = m_player->get<CTransform>();

		Vec2f velocity(0.0f, 0.0f);
		if (input.up)    velocity.y -= 1.0f;
		if (input.down)  velocity.y += 1.0f;
		if (input.left)  velocity.x -= 1.0f;
		if (input.right) velocity.x += 1.0f;

		// Normalize to prevent faster diagonal movement
		if (velocity.x != 0.0f || velocity.y != 0.0f) {
			velocity.normalize();
			float frameSpeed = m_playerConfig.SSpeed / 60.0f; // Scale Speed to Frame
			transform.velocity = velocity * frameSpeed;
		}
		else {
			transform.velocity = Vec2f(0.0f, 0.0f);
		}
	}
	// Apply velocity to position for all entities
	for (auto e : m_entities.getEntities()) {
		if (e->has<CTransform>()) {
			auto& t = e->get<CTransform>();
			t.position += t.velocity;
		}
	}
}

void Engine::sCollision() {
	// detect and handle collisions between entities with collision components, such as player-enemy, player-projectile, player-wall, enemy-wall collisions, using the collision system logic.

	float winWidth = static_cast<float>(m_window.getSize().x);
	float winHeight = static_cast<float>(m_window.getSize().y);

	// Player vs Wall Bounds
	if (m_player && m_player->isActive()) {
		auto& pTransform = m_player->get<CTransform>();
		auto& pCol = m_player->get<CCollision>();

		if (pTransform.position.x - pCol.radius < 0) pTransform.position.x = pCol.radius;
		if (pTransform.position.x + pCol.radius > winWidth) pTransform.position.x = winWidth - pCol.radius;
		if (pTransform.position.y - pCol.radius < 0) pTransform.position.y = pCol.radius;
		if (pTransform.position.y + pCol.radius > winHeight) pTransform.position.y = winHeight - pCol.radius;
	}

	// Enemy & Small Enemy vs Wall (Bounce Effect)
	auto checkWallCollision = [&](const std::string& tag) {
		for (auto& e : m_entities.getEntitiesByTag(tag)) {
			auto& t = e->get<CTransform>();
			auto& c = e->get<CCollision>();

			if (t.position.x - c.radius < 0) { t.position.x = c.radius; t.velocity.x *= -1.0f; }
			if (t.position.x + c.radius > winWidth) { t.position.x = winWidth - c.radius; t.velocity.x *= -1.0f; }
			if (t.position.y - c.radius < 0) { t.position.y = c.radius; t.velocity.y *= -1.0f; }
			if (t.position.y + c.radius > winHeight) { t.position.y = winHeight - c.radius; t.velocity.y *= -1.0f; }
		}
		};
	checkWallCollision("enemy");
	checkWallCollision("small_enemy");

	// Projectiles vs Wall (Destroy Object)
	for (auto& p : m_entities.getEntitiesByTag("projectile")) {
		auto& t = p->get<CTransform>();
		auto& c = p->get<CCollision>();
		if (t.position.x - c.radius < 0 || t.position.x + c.radius > winWidth ||
			t.position.y - c.radius < 0 || t.position.y + c.radius > winHeight) {
			p->destroy();
		}
	}

	// Projectile vs Enemies
	for (auto& proj : m_entities.getEntitiesByTag("projectile")) {
		if (!proj->isActive()) continue;

		auto checkProjectileHit = [&](const std::string& tag, bool isSmall) {
			for (auto& enemy : m_entities.getEntitiesByTag(tag)) {
				if (!enemy->isActive() || !proj->isActive()) continue;

				auto& pTrans = proj->get<CTransform>();
				auto& eTrans = enemy->get<CTransform>();
				auto& pCol = proj->get<CCollision>();
				auto& eCol = enemy->get<CCollision>();

				float distSq = pTrans.position.distanceSq(eTrans.position);
				float radSum = pCol.radius + eCol.radius;

				if (distSq < (radSum * radSum)) {
					proj->destroy();

					// Apply damage to the enemy
					enemy->get<CHealth>().currentHealth -= proj->get<CDamage>().damageValue;

					if (enemy->get<CHealth>().currentHealth <= 0.0f) {
						m_currentScore += enemy->get<CScoreValue>().scoreValue;
						enemy->destroy();
						if (!isSmall) spawnSmallEnemies(enemy);
					}
					break; // Projectile is dead, stop checking enemies for it
				}
			}
			};

		checkProjectileHit("enemy", false);
		checkProjectileHit("small_enemy", true);
	}

	// Player vs Enemies
	if (m_player && m_player->isActive()) {
		auto& pTrans = m_player->get<CTransform>();
		auto& pCol = m_player->get<CCollision>();
		auto& pHealth = m_player->get<CHealth>();

		auto checkPlayerHit = [&](const std::string& tag, bool isSmall) {
			for (auto& enemy : m_entities.getEntitiesByTag(tag)) {
				if (!enemy->isActive()) continue;

				auto& eTrans = enemy->get<CTransform>();
				auto& eCol = enemy->get<CCollision>();

				float distSq = pTrans.position.distanceSq(eTrans.position);
				float radSum = pCol.radius + eCol.radius;

				if (distSq < (radSum * radSum)) {
					enemy->destroy();
					if (!isSmall) spawnSmallEnemies(enemy);

					// Apply damage to the player
					pHealth.currentHealth -= enemy->get<CDamage>().damageValue;

					// Check for Player Death
					if (pHealth.currentHealth <= 0.0f) {
						// Reset player position to center
						pTrans.position = Vec2f(winWidth / 2.0f, winHeight / 2.0f);
						// Reset player health
						pHealth.currentHealth = pHealth.totalHealth;
						// Reset score
						m_currentScore = 0;
					}
				}
			}
			};

		checkPlayerHit("enemy", true);
		checkPlayerHit("small_enemy", true);
	}
			// Destroy the enemy that collided with the player
}

void Engine::sLifespan() {
	// update the remaining lifespan of entities with a lifespan component and mark them for destruction when their lifespan expires, using the lifespan system logic.
	// should update the entity's color based on remaining lifespan
	// when lifespan is 0 destroy the entity
	for (auto& e : m_entities.getEntities()) {
		if (e->has<CLifespan>()) {
			auto& life = e->get<CLifespan>();
			life.remainingLifespan--;

			if (life.remainingLifespan <= 0) {
				e->destroy();
			}
			else if (e->has<CShape>()) {
				auto& shape = e->get<CShape>().circle;

				// Extract raw colors
				sf::Color fill = shape.getFillColor();
				sf::Color out = shape.getOutlineColor();

				// Alpha blend formula over remaining lifespan
				float alphaRatio = static_cast<float>(life.remainingLifespan) / static_cast<float>(life.totalLifespan);

				fill.a = static_cast<sf::Uint8>(255.0f * alphaRatio);
				out.a = static_cast<sf::Uint8>(255.0f * alphaRatio);

				shape.setFillColor(fill);
				shape.setOutlineColor(out);
			}
		}
	}
}

void Engine::sEnemySpawner() {
	// spawn enemies based on their spawn rate and the time elapsed since the last spawn, using the enemy configuration data and the enemy spawner system logic.
	if (m_currentFrame - m_lastEnemySpawnFrame >= 120) {
		spawnEnemy();
	}
}

/* Not needed at this time:
void Engine::sProjectileSpawner() {
}
*/

void Engine::sGUI() {
	// render the in-game debug UI using ImGui.
	ImGui::Begin("Geometry Wars Plus Debugger");
	ImGui::Text("Score: %d", m_currentScore);
	if (ImGui::Button("Spawn Enemy")) spawnEnemy();
	ImGui::End();
}

void Engine::sRender() {
	// render all entities with a shape component and render the player's score and game state messages using the text component.
	// window clear, update, draw, display
	// make the shapes that aren't player rotate
	// make one of the vericies of the player entity "follow" the mouse
	m_window.clear();

	// Player always tracks the mouse pointer
	if (m_player && m_player->isActive()) {
		auto mousePos = sf::Mouse::getPosition(m_window);
		auto& pTrans = m_player->get<CTransform>();

		Vec2f diff = Vec2f((float)mousePos.x, (float)mousePos.y) - pTrans.position;
		// atan2 calculates angle in radians, we convert to degrees for sfml
		pTrans.angle = std::atan2(diff.y, diff.x) * (180.0f / 3.14159f);
	}

	for (auto e : m_entities.getEntities()) {
		if (e->has<CShape>() && e->has<CTransform>()) {
			auto& t = e->get<CTransform>();
			auto& shapeComp = e->get<CShape>();

			// Make non-player shapes slowly rotate over time
			if (e->getTag() != "player") {
				t.angle += 1.5f;
			}

			shapeComp.circle.setPosition(t.position.x, t.position.y);
			shapeComp.circle.setRotation(t.angle);

			m_window.draw(shapeComp.circle);
		}
	}

	ImGui::SFML::Render(m_window);
	m_window.display();
}

void Engine::sInput() {
	// handle player input and update the player's input component accordingly.
	// handle if the mouse is on the ImGui window.
	// poll events, key presses and releases, mouse button presses and releases, mouse position, etc.. if statements
	sf::Event event;
	while (m_window.pollEvent(event)) {
		ImGui::SFML::ProcessEvent(event);

		if (event.type == sf::Event::Closed) {
			m_running = false;
		}

		// Let ImGui "eat" inputs if a window is active.
		if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard) {
			continue;
		}

		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape) m_running = false;
			if (event.key.code == sf::Keyboard::P) setPaused(!m_paused);

			if (m_player && m_player->isActive()) {
				auto& input = m_player->get<CInput>();
				if (event.key.code == sf::Keyboard::W) input.up = true;
				if (event.key.code == sf::Keyboard::S) input.down = true;
				if (event.key.code == sf::Keyboard::A) input.left = true;
				if (event.key.code == sf::Keyboard::D) input.right = true;
			}
		}

		if (event.type == sf::Event::KeyReleased) {
			if (m_player && m_player->isActive()) {
				auto& input = m_player->get<CInput>();
				if (event.key.code == sf::Keyboard::W) input.up = false;
				if (event.key.code == sf::Keyboard::S) input.down = false;
				if (event.key.code == sf::Keyboard::A) input.left = false;
				if (event.key.code == sf::Keyboard::D) input.right = false;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed) {
			if (m_player && m_player->isActive() && !m_paused) {
				Vec2f mousePos((float)event.mouseButton.x, (float)event.mouseButton.y);
				if (event.mouseButton.button == sf::Mouse::Left) {
					spawnProjectile(m_player, mousePos);
				}
				else if (event.mouseButton.button == sf::Mouse::Right) {
					spawnSpecialAbility(m_player);
				}
			}
		}
	}
}