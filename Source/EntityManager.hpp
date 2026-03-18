#pragma once // Prevents multiple inclusions of this header file.
#include "Entity.hpp" // Include the header file that contains the declaration of the Entity class and its variables and functions.
#include <vector> // Include the vector library for storing and managing entities in the EntityManager.
#include <map> // Include the map library for associating string tags with specific lists of entities.
#include <memory> // Include the memory library for smart pointers. Required for automatic memory management of entities within the EntityManager.
#include <string> // Include the string library for using std::string. Required for the tag property used for identifying and categorizing entities.

// Create type aliases for commonly used data structures in the EntityManager to improve code readability and maintainability.
// Define a type alias for a vector of shared pointers to Entity objects, which will be used to store and manage entities in the EntityManager.
using EntityVector = std::vector<std::shared_ptr<Entity>>;
// Define a type alias for a map that associates string tags with vectors of shared pointers to Entity objects, allowing for categorization and easy retrieval of entities based on their tags.
using EntityMap = std::map<std::string, EntityVector>;

class EntityManager { // Defines the EntityManager class, which is responsible for creating, managing, destroying and organizing entities in the RizzyEngine.
	private: // Private members that are only accessible within the EntityManager class. They are used to store and manage the entities in the game world.
		EntityVector m_entities; // A vector that holds shared pointers to all entities in the game world.
		EntityVector m_entitiesToAdd; // A vector that holds shared pointers to entities that are pending addition ("Waiting Room" for entities to be added).
		EntityMap m_entitiesByTag; // A map that associates string tags with vectors of shared pointers to Entity objects.
		size_t m_totalEntitiesCreated = 0; // A counter to keep track of the total number of entities created, used for assigning unique IDs to new entities.

	// Private helper function to clean up inactive entities from a vector without repeating code.
	void removeInactiveEntities(EntityVector& entities);

	public: // Public members that can be accessed from outside the EntityManager class. Provides the interface for creating, managing, destroying, and retrieving entities.
		EntityManager(); // Default constructor for the EntityManager class that initializes the entity manager and its data structures.

		/*Function to update the EntityManager, which includes adding pending entities and removing inactive entities from the main entity vector and tag - based vectors.
		Called at the start of every frame to update the entity vectors before the Systems process the entities for that frame.*/
		void update(); // This function is the "heart" of the EntityManager.

		std::shared_ptr<Entity> createEntity(const std::string& tag); // Factory function to create a new entity with a specified tag.
		const EntityVector& getEntities() const; // Function to return a const reference to the vector of all active entities in the game world.
		const EntityVector& getEntitiesByTag(const std::string& tag) const; // Function to return a const reference to the vector of active entities that match a specific tag.
};