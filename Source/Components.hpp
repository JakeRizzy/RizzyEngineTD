#pragma once // Prevents multiple inclusions of this header file.
#include "Vec2.hpp" // Include the file that contains the custom Vec2 Utility Math class.
#include <SFML/Graphics.hpp> // Include the SFML Graphics library for SFMLs types and functionality (Shapes, Color, Text).

struct CTransform { // Component for the trasformation properties of entities.
	Vec2<float> position = { 0.00f, 0.00f }; // Initialize a Vec2 variable called "position" to hold the x and y coordinates of an entity, with default values of (0, 0).
	Vec2<float> velocity = { 0.00f, 0.00f }; // Initialize a Vec2 variable called "velocity" to hold the velocity along the x and y axis for an entity, with default values of (0, 0).
	float angle = 0.00f; // Initialize a float variable called "angle" to hold the rotation angle of an entity, with a default value of 0 degrees.

	// Parameterized constructor for CTransform that takes in a position, velocity, and angle to initialize the component's properties.
	CTransform(const Vec2<float>& pos, const Vec2<float>& vel, float ang) : position(pos), velocity(vel), angle(ang) {}
};

struct CShape { // Component for the shape properties of entities.
	sf::CircleShape circle; // Create a circle shape object called "circle"

	// Parameterized constructor for CShape that takes in a radius, number of vertices, fill color, outline color, and outline thickness to initialize the circle shape component's properties.
	// SFML constroctor for circle shapes only takes in circle(float radius, size_t pointCount).	
	CShape(float radius, int vertices, const sf::Color& fillColor, const sf::Color& outlineColor, float outlineThickness) : circle(radius, vertices) {
		circle.setFillColor(fillColor); // Set the circle's fill color to the specified fill color.
		circle.setOutlineColor(outlineColor); // Set the circle's outline color to the specified outline color.
		circle.setOutlineThickness(outlineThickness); // Set the circle's outline thickness to the specified outline thickness.
		circle.setOrigin(radius, radius); // Set the circle's origin to be its center based on its radius.
	}
};

struct CCollision { // Component for the collision properties of entities.
	float radius = 0.00f; // Initialize a float variable called "radius" to hold the radius of an entity's collision circle, with a default value of 0.

	// Parameterized constructor for CCollision that takes in a radius to initialize the collision component's properties.
	CCollision(float rad) : radius(rad) {} 
};

struct CLifespan { // Component for the lifespan properties of entities.
	int totalLifespan = 0, remainingLifespan = 0; // Initialize int variables to hold the total lifespan and remaining lifespan of an entity, with default values of 0.

	// Parameterized constructor for CLifespan that takes in a total lifespan in frames to initialize the lifespan component's properties.
	CLifespan (int totalLifespanFrames) : totalLifespan(totalLifespanFrames), remainingLifespan(totalLifespanFrames) {}
};

struct CInput { // Component for the input properties of entities.
	// Initialize boolean variables to hold the state of input controls for an entity, with default values of false (not pressed).
	bool up = false, down = false, left = false, right = false, shoot = false, special = false;
	
	// Default constructor for CInput that initializes the input component's properties to their default values (
	CInput() {}
};

//Optional Components based on feature needs for entities in the engine to be commented out if not needed:

/*
struct CHealth { // Component for the health properties of entities.
	float totalHealth = 10.00f, currentHealth = 10.00f; // Initialize float variables to hold the total health and current health of an entity, with default values of 10.00f.

	// Parameterized constructor for CHealth that takes in a health value to initialize the health component's properties.
	CHealth(float health) : totalHealth(health), currentHealth(health) {}
};

struct CDamage { // Component for the damage propeties of entities.
	float damageValue = 0.00f; // Initialize a float variable called "damageValue" to hold the damage value of an entity, with a default value of 0.00f.
	
	// Parameterized constructor for CDamage that takes in a damage value to initialize the damage component's properties.
	CDamage(float damage) : damageValue(damage) {}
};
*/

struct CScoreValue { // Component for the score value properties of entities.
	int scoreValue = 0; // Initialize an int variable called "scoreValue" to hold the score value of an entity, with a default value of 0.
	
	// Parameterized constructor for CScoreValue that takes in a score value to initialize the score value component's properties.
	CScoreValue(int score) : scoreValue(score) {}
};