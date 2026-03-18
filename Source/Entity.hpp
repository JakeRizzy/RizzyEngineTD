#pragma once // Prevents multiple inclusions of this header file.
#include "Components.hpp" // Include the file that contains the component structures for entities.
#include <memory> // Include the memory library for smart pointers. Required for automatic memory management of components within entities.
#include <string> // Include the string library for using std::string. Required for the tag property used for identifying and categorizing entities.

class Entity { // Defines the base class for all game objects in the RizzyEngine.
	friend class EntityManager; // Declare the EntityManager class as a friend of the Entity class, allowing it to access private members of Entity for managing entities and their components.

	private: // Private members that only the EntityManager will interact with.
		std::string m_tag = "untagged"; // Initialize a string label used for identifying categorizing entities (i.e., "Player", "Enemy", "Projectile", etc.) with a default value of "untagged".
		size_t m_id = 0; // Initialize a unique identifier for each entity, which can be used for tracking and referencing entities, with a default value of 0.
		bool m_isActive = true; // Initialize a boolean flag to indicate whether the entity is active in the game world, with a default value of true.

		// Private constructor for the Entity class that takes in an id and tag to initialize the entity's properties.
		// This constructor is private to ensure that entities can only be created and managed through the EntityManager.
		Entity(const size_t id, const std::string& tag);

	public: // Public members that the Systems (i.e. sRender, sCollision, etc.) will interact with.
		// Component Pointers: We use shared_ptr so components are deleted automatically when the Entity dies.
		// If an entity doesn't have a specific feature (like input), that pointer stays as 'nullptr'.
		std::shared_ptr<CTransform> cTransform; // Pointer to the CTransform component, which holds transformation properties like position, velocity, and angle.
		std::shared_ptr<CShape> cShape; // Pointer to the CShape component, which holds shape properties like the SFML shape object and its visual properties.
		std::shared_ptr<CCollision> cCollision; // Pointer to the CCollision component, which holds collision properties like the radius of the entity's collision circle.
		std::shared_ptr<CLifespan> cLifespan; // Pointer to the CLifespan component, which holds lifespan properties like total and remaining lifespan in frames.
		std::shared_ptr<CInput> cInput; // Pointer to the CInput component, which holds input properties like the state of input controls for the entity.

		// Optional Components based on feature needs for entities in the engine to be commented out if not needed:
		/*
		std::shared_ptr<CHealth> cHealth; // Pointer to the CHealth component, which holds health properties like total and current health of the entity.
		std::shared_ptr<CDamage> cDamage; // Pointer to the CDamage component, which holds damage properties like the damage value of the entity.
		*/
		std::shared_ptr<CScoreValue> cScoreValue; // Pointer to the CScoreValue component, which holds scoring properties like the score value of the entity when destroyed.

		// Public helper functions for the Systems to interact with the entity's properties and components.
		bool isActive() const; // Function that returns whether the entity is active in the game world. True if active, false if not.
		const std::string& getTag() const; // Function that returns a const reference to the entity's tag string for identifying and categorizing the entity.
		const size_t getId() const; // Function that returns the unique identifier of the entity.
		void destroy(); // Function to set m_isActive to false, effectively marking the entity for deletion by the EntityManager.
};