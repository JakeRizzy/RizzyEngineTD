/*
Product Owner: Jacob Risoldi
Product Name: RizzyEngine
Version: 0.5.0: Geometry Wars Plus
Version Date: March 2026
Language: C++ 20
Toolchain: SFML 2.6.1, ImGui 1.89.9, ImGui-SFML 2.6
*/

/*
#include "Vec2.hpp"
#include "Components.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"
*/
#include "Engine.h"
#include <iostream>

int main() {
    
    Engine game("config.txt"); // Create an instance of the Engine class, passing in the path to the configuration file for initializing the engine's settings and properties.
    game.run(); // Call the run function of the game instance to start the main game loop and handle the window, rendering, the EntityManager, and all gameplay systems.
    return 0;
    

    /*Main Loop Test*/
    /*
    sf::RenderWindow window(sf::VideoMode(1280, 720), "FR-0: Moving Core Loop");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    sf::CircleShape shape(50.0f);
    shape.setFillColor(sf::Color::Cyan);

    sf::Vector2f velocity(200.0f, 200.0f);
    
    sf::Clock deltaClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) window.close();
        }

        // UPDATE Logic
        sf::Time dt = deltaClock.restart();
        ImGui::SFML::Update(window, dt);

        shape.move(velocity * dt.asSeconds());

        // Screen Bounce (Simple Validation)
        if (shape.getPosition().x < 0 || shape.getPosition().x + 100 > 1280) velocity.x *= -1;
        if (shape.getPosition().y < 0 || shape.getPosition().y + 100 > 720)  velocity.y *= -1;

        // RENDER Logic
        window.clear();
        window.draw(shape);

        ImGui::Begin("FR-0 Status");
        ImGui::Text("Core Loop: ACTIVE");
        ImGui::Text("Position: %.1f, %.1f", shape.getPosition().x, shape.getPosition().y);
        ImGui::End();

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
    */

    /*Vec2 Test*/
	/*
    std::cout << "--- Starting Vec2 Tests ---\n"; // Print a message to indicate the start of the Vec2 tests.
    std::cout << std::boolalpha; // Prints true/false instead of 1/0
	Vec2f a(10.0f, 10.0f); // Create a Vec2f variable called "a" and initialize it with the values (10.0f, 10.0f) to represent a 2D vector with x and y coordinates of 10.
	Vec2f b(20.0f, 30.0f); // Create a Vec2f variable called "b" and initialize it with the values (20.0f, 30.0f) to represent a 2D vector with x and y coordinates of 20 and 30, respectively.
	Vec2i c(30, 40); // Create a Vec2i variable called "c" and initialize it with the values (5, 15) to represent a 2D vector with x and y coordinates of 5 and 15, respectively.
	Vec2 d(40.0, 50.0); // Create a Vec2 variable called "d" and initialize it with the values (0.0, 0.0) to represent a 2D vector with x and y coordinates of 0.
    // Test Equality
    std::cout << "Test Equality:               " << (a == Vec2f(10.0f, 10.0f)) << "\n";

    // Test Inequality
    std::cout << "Test Inequality:             " << (a != b) << "\n";

    // Test Addition
    std::cout << "Test Addition:               " << ((a + b) == Vec2f(30.0f, 40.0f)) << "\n";

    // Test Subtraction
    std::cout << "Test Subtraction:            " << ((b - a) == Vec2f(10.0f, 20.0f)) << "\n";

    // Test Scalar Multiplication
    std::cout << "Test Scalar Multiplication:  " << ((a * 2.0f) == Vec2f(20.0f, 20.0f)) << "\n";

    // Test Scalar Division
    std::cout << "Test Scalar Division:        " << ((b / 2.0f) == Vec2f(10.0f, 15.0f)) << "\n";

    // Test Addition Assignment
    Vec2f aCopy = a; aCopy += b;
    std::cout << "Test Addition Assignment:    " << (aCopy == Vec2f(30.0f, 40.0f)) << "\n";

    // Test Subtraction Assignment
    aCopy -= b;
    std::cout << "Test Subtraction Assignment: " << (aCopy == a) << "\n";

    // Test Multiplication Assignment
    Vec2f mCopy = a; mCopy *= 2.0f;
    std::cout << "Test Multi Assignment:       " << (mCopy == Vec2f(20.0f, 20.0f)) << "\n";

    // Test Division Assignment
    Vec2f dCopy = b; dCopy /= 2.0f;
    std::cout << "Test Division Assignment:    " << (dCopy == Vec2f(10.0f, 15.0f)) << "\n";

    // Test Length
    std::cout << "Test Length:                 " << (c.length() == 50.0f) << "\n";
    std::cout << "Test LengthSq:               " << (c.lengthSq() == 2500.0f) << "\n";

    // Test Distance
    std::cout << "Test DistanceSq:             " << (a.distanceSq(b) == 500.0f) << "\n";

    // Test Normalization
    Vec2f n(10.0f, 0.0f);
    n.normalize();
    std::cout << "Test Normalization:          " << (n == Vec2f(1, 0)) << "\n";

    std::cout << "--- Vec2 Tests Complete ---\n\n";
    */

    /*Components Test*/
    /*
    std::cout << "--- Starting Components Tests ---\n";
    std::cout << std::boolalpha; // Prints true/false instead of 1/0
    // Test CTransform initialization and Vec2 integration
    CTransform transform(Vec2f(100.0f, 100.0f), Vec2f(5.0f, 5.0f), 45.0f);
    std::cout << "Transform exists:    " << (transform.exists = true) << "\n"; // Manually setting for test

    // Test if we can use our Vec2 operators through the component
    transform.position += transform.velocity;
    std::cout << "Movement Test:       " << (transform.position == Vec2f(105.0f, 105.0f)) << "\n";

    // Test CShape SFML properties
    CShape shape(20.0f, 3, sf::Color::Red, sf::Color::White, 2.0f);
    std::cout << "Shape Radius:        " << (shape.circle.getRadius() == 20.0f) << "\n";
    std::cout << "Shape Points:        " << (shape.circle.getPointCount() == 3) << "\n";

    // Test CLifespan logic
    CLifespan life(60); // 60 frames
    life.remainingLifespan -= 10;
    std::cout << "Lifespan Reduction:  " << (life.remainingLifespan == 50) << "\n";

    // Test CInput defaults
    CInput input;
    std::cout << "Input Default (up):  " << (input.up == false) << "\n";
    input.up = true;
    std::cout << "Input Change (up):   " << (input.up == true) << "\n";

    std::cout << "--- Components Tests Complete ---\n\n";
    */

    /*Entity Test*/
    /*
    // Note: For this test to work before EntityManager is finished, the Entity constructor must be temporarily moved to 'public'.
    std::cout << "--- Starting Entity Tests ---\n";
    std::cout << std::boolalpha;

    Entity e(1, "Player");

    std::cout << "Entity ID:           " << (e.getId() == 1) << "\n";
    std::cout << "Entity Tag:          " << (e.getTag() == "Player") << "\n";

    // Test Component Addition
    e.add<CTransform>(Vec2f(100.0f, 200.0f), Vec2f(1.0f, 1.0f), 0.0f);
    std::cout << "Has CTransform:      " << e.has<CTransform>() << "\n";

    // Test Component Retrieval
    auto& pos = e.get<CTransform>().position;
    std::cout << "Get Position X:      " << (pos.x == 100.0f) << "\n";

    // Test Component Modification
    pos.x = 500.0f;
    std::cout << "Modify Position:     " << (e.get<CTransform>().position.x == 500.0f) << "\n";

    // Test "exists" flag logic (We haven't added a CShape yet, so it should be false.
    std::cout << "Has CShape (none):   " << (e.has<CShape>() == false) << "\n";

    e.add<CShape>(10.0f, 32, sf::Color::Blue, sf::Color::White, 2.0f);
    std::cout << "Has CShape (added):  " << e.has<CShape>() << "\n";

    // Test Component Removal
    e.remove<CTransform>();
    std::cout << "Has CTransform:      " << (e.has<CTransform>() == false) << "\n";

    // Test Const Correctness
    const Entity& constE = e;
    // This calls the const version of get<T>()
    std::cout << "Const Access:        " << constE.get<CShape>().exists << "\n";

    std::cout << "--- Entity Tests Complete ---\n\n";
    */

    /*EntityManager Test*/
    /*
    std::cout << "--- Starting EntityManager Tests ---\n";
    std::cout << std::boolalpha;

    EntityManager entityManager;

    // Test Creation & Waiting Room logic
    auto player = entityManager.createEntity("Player");

    std::cout << "Step 1: Entity Created\n";
    std::cout << "Entity in main list? (should be false): " << (entityManager.getEntities().size() > 0) << "\n";
    std::cout << "Entity in tag map?   (should be false): " << (entityManager.getEntitiesByTag("Player").size() > 0) << "\n";

    // Test Synchronization (Update)
    entityManager.update();
    std::cout << "\nStep 2: After update()\n";
    std::cout << "Entity in main list? (should be true):  " << (entityManager.getEntities().size() == 1) << "\n";
    std::cout << "Entity in tag map?   (should be true):  " << (entityManager.getEntitiesByTag("Player").size() == 1) << "\n";

    // Test Map Pollution Prevention
    auto& nonExistent = entityManager.getEntitiesByTag("Boss");
    std::cout << "\nStep 3: Map Pollution Test\n";
    std::cout << "Searching for 'Boss' returned empty:   " << nonExistent.empty() << "\n";
    // If you had map pollution, the map size would be 2 now (Player and Boss)
    // We can't easily check map size without getEntityMap(), but we know it's safe!

    // Test Inactive Removal
    player->destroy(); // Mark as inactive
    std::cout << "\nStep 4: Marking Player for Destruction\n";
    std::cout << "Is player active?    (should be false): " << player->isActive() << "\n";
    std::cout << "Still in list before update? (true):   " << (entityManager.getEntities().size() == 1) << "\n";

    entityManager.update();
    std::cout << "Still in list after update?  (false):  " << (entityManager.getEntities().size() == 0) << "\n";
    std::cout << "Still in map after update?   (false):  " << (entityManager.getEntitiesByTag("Player").size() == 0) << "\n";

    // Test Multiple Entities
    entityManager.createEntity("Enemy");
    entityManager.createEntity("Enemy");
    entityManager.createEntity("Enemy");
    entityManager.update();
    std::cout << "\nStep 5: Bulk Creation\n";
    std::cout << "Total Entities:      (should be 3):     " << entityManager.getEntities().size() << "\n";
    std::cout << "Enemy Tag Count:     (should be 3):     " << entityManager.getEntitiesByTag("Enemy").size() << "\n";

    std::cout << "--- EntityManager Tests Complete ---\n\n";
    */

    /*ECS Test*/
    /*
    std::cout << "--- Starting 5-Frame ECS Integration Test ---\n";
    EntityManager em;

    // Create a Projectile (Simulating a trap in the escape room)
    // Starting at (0,0), moving at (10, 5) per frame
    auto projectile = em.createEntity("Projectile");
    projectile->add<CTransform>(Vec2f(0.0f, 0.0f), Vec2f(10.0f, 5.0f), 0.0f);

    // Run the simulation for 5 frames
    for (int frame = 1; frame <= 5; ++frame) {
    // Essential: Update the manager to sync the 'Waiting Room' 
    // and clear out any destroyed entities from the previous frame.
    em.update();

        // Simulate sMovement (Movement System)
        for (auto& e : em.getEntities()) {
            if (e->has<CTransform>()) {
                auto& transform = e->get<CTransform>();
                transform.position += transform.velocity;
            }
        }

        Vec2f currentPos = projectile->get<CTransform>().position;
        std::cout << "Frame " << frame << " Position: (" << currentPos.x << ", " << currentPos.y << ")\n";
    }

    // Validation
    // After 5 frames of (10, 5) velocity, we expect (50, 25)
    Vec2f finalPos = projectile->get<CTransform>().position;
    bool pass = (finalPos.x == 50.0f && finalPos.y == 25.0f);

    std::cout << "\nFinal Position: (" << finalPos.x << ", " << finalPos.y << ")\n";
    std::cout << "Test Result:    " << (pass ? "PASS" : "FAIL") << "\n";
    std::cout << "--- ECS Test Complete ---\n\n";
    */

    /*Engine Test*/

}