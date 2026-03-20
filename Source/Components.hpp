#pragma once // Prevents multiple inclusions of this header file.
#include "Vec2.hpp" // Include the file that contains the custom Vec2 Utility Math class.

struct Component { // Base component struct that all other components will inherit from.
	bool exists = false; // Initialize a boolean variable called "exists" to indicate whether the component exists for an entity, with a default value of false.
};

struct CTransform : public Component { // Component for the transformation properties of entities, inheriting from the base Component struct.
	Vec2f position = { 0.00f, 0.00f }; // Initialize a Vec2 variable called "position" to hold the x and y coordinates of an entity, with default values of (0, 0).
	Vec2f velocity = { 0.00f, 0.00f }; // Initialize a Vec2 variable called "velocity" to hold the velocity along the x and y axis for an entity, with default values of (0, 0).
	float angle = 0.00f; // Initialize a float variable called "angle" to hold the rotation angle of an entity, with a default value of 0 degrees.

	CTransform() = default; // Default constructor for CTransform that initializes the component's properties to their default values (position (0, 0), velocity (0, 0), and angle 0 degrees).
	// Parameterized constructor for CTransform that takes in a position, velocity, and angle to initialize the component's properties.
	CTransform(const Vec2f& pos, const Vec2f& vel, float ang) : position(pos), velocity(vel), angle(ang) {}
};

struct CShape : public Component { // Component for the shape properties of entities, inheriting from the base Component struct.
	sf::CircleShape circle; // Create a circle shape object called "circle"

	CShape() = default; // Default constructor for CShape that initializes the circle shape component's properties to their default values.
	// Parameterized constructor for CShape that takes in a radius, number of vertices, fill color, outline color, and outline thickness to initialize the circle shape component's properties.
	// SFML constructor for circle shapes only takes in circle(float radius, size_t pointCount).	
	CShape(float outlineThickness, float radius, int vertices, const sf::Color& fillColor, const sf::Color& outlineColor) : circle(radius, vertices) {
		circle.setOutlineThickness(outlineThickness); // Set the circle's outline thickness to the specified outline thickness.
		circle.setFillColor(fillColor); // Set the circle's fill color to the specified fill color.
		circle.setOutlineColor(outlineColor); // Set the circle's outline color to the specified outline color.
		circle.setOrigin(radius, radius); // Set the circle's origin to be its center based on its radius.
	}
};

struct CCollision : public Component { // Component for the collision properties of entities, inheriting from the base Component struct.
	float radius = 0.00f; // Initialize a float variable called "radius" to hold the radius of an entity's collision circle, with a default value of 0.

	CCollision() = default; // Default constructor for CCollision that initializes the collision component's properties to their default values (radius 0).
	// Parameterized constructor for CCollision that takes in a radius to initialize the collision component's properties.
	CCollision(float rad) : radius(rad) {} 
};

struct CLifespan : public Component { // Component for the lifespan properties of entities, inheriting from the base Component struct.
	int totalLifespan = 0, remainingLifespan = 0; // Initialize int variables to hold the total lifespan and remaining lifespan of an entity, with default values of 0.

	CLifespan() = default; // Default constructor for CLifespan that initializes the lifespan component's properties to their default values (total and remaining lifespan of 0).
	// Parameterized constructor for CLifespan that takes in a total lifespan in frames to initialize the lifespan component's properties.
	CLifespan (int totalLifespanFrames) : totalLifespan(totalLifespanFrames), remainingLifespan(totalLifespanFrames) {}
};

struct CInput : public Component { // Component for the input properties of entities, inheriting from the base Component struct.
	// Initialize boolean variables to hold the state of input controls for an entity, with default values of false (not pressed).
	bool up = false, down = false, left = false, right = false, pause = false, interact = false, shoot = false, special = false;
	
	// Default constructor for CInput that initializes the input component's properties to their default values (
	CInput() = default;
};

//Optional Components based on feature needs for entities in the engine to be commented out if not needed:
struct CHealth : public Component { // Component for the health properties of entities, inheriting from the base Component struct.
	float totalHealth = 10.00f, currentHealth = 10.00f; // Initialize float variables to hold the total health and current health of an entity, with default values of 10.00f.

	CHealth() = default; // Default constructor for CHealth that initializes the health component's properties to their default values (total and current health of 10.00f).
	// Parameterized constructor for CHealth that takes in a health value to initialize the health component's properties.
	CHealth(float health) : totalHealth(health), currentHealth(health) {}
};

struct CDamage : public Component { // Component for the damage properties of entities, inheriting from the base Component struct.
	float damageValue = 0.00f; // Initialize a float variable called "damageValue" to hold the damage value of an entity, with a default value of 0.00f.
	
	CDamage() = default; // Default constructor for CDamage that initializes the damage component's properties to their default values (damage value of 0.00f).
	// Parameterized constructor for CDamage that takes in a damage value to initialize the damage component's properties.
	CDamage(float damage) : damageValue(damage) {}
};

struct CScoreValue : public Component { // Component for the scoring properties of entities, inheriting from the base Component struct.
	int scoreValue = 0; // Initialize an int variable called "scoreValue" to hold the score value of an entity, with a default value of 0.
	
	CScoreValue() = default; // Default constructor for CScoreValue that initializes the score value component's properties to their default values (score value of 0).
	// Parameterized constructor for CScoreValue that takes in a score value to initialize the score value component's properties.
	CScoreValue(int score) : scoreValue(score) {}
};

struct CSpecialAbility : public Component { // Component for the input properties of entities, inheriting from the base Component struct.
	// Initialize boolean variables to hold the state of input controls for an entity, with default values of false (not pressed).
	float totalCooldown = 0.00f; // Initialize a float variable called "cooldownTime" to hold the cooldown time of an entity's special ability, with a default value of 0.00f.
	float remainingCooldown = 0.00f; // Initialize a float variable called "cooldownRemaining" to hold the remaining cooldown time of an entity's special ability, with a default value of 0.00f.

	// Default constructor for CInput that initializes the input component's properties to their default values (
	CSpecialAbility() = default;
	// Parameterized constructor for CInput that takes in a cooldown time to initialize the special ability component's properties.
	CSpecialAbility(float cooldownTime) : totalCooldown(cooldownTime), remainingCooldown(0.00f) {}
};