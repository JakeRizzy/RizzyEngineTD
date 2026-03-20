#pragma once // Prevents multiple inclusions of this header file.
#include "Entity.hpp" // Include the header file that contains the declaration of the Entity class and its variables and functions.
#include <vector> // Include the vector library for storing and managing entities in the EntityManager.
#include <map> // Include the map library for associating string tags with specific lists of entities.
#include <memory> // Include the memory library for smart pointers. Required for automatic memory management of entities within the EntityManager.
#include <string> // Include the string library for using std::string. Required for the tag property used for identifying and categorizing entities.
#include <algorithm> // Include the algorithm library for using the std::remove_if function to remove inactive entities from vectors.

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
	void removeInactiveEntities(EntityVector& entities) { // This function is called by the update() function and removes any entities that are marked as inactive.
		std::erase_if(entities, [](const auto& entity) { // Lambda function that takes a shared pointer to an Entity and returns true if the entity is inactive.
			return !entity->isActive(); // Use the std::erase_if algorithm to remove entities from the vector that are marked as inactive (where isActive() returns false).
			});
	}

public: // Public members that can be accessed from outside the EntityManager class. Provides the interface for creating, managing, destroying, and retrieving entities.
	EntityManager() = default; // Default constructor for the EntityManager class that initializes the entity manager and its data structures.

	/*Function to update the EntityManager, which includes adding pending entities and removing inactive entities from the main entity vector and tag - based vectors.
	Called at the start of every frame to update the entity vectors before the Systems process the entities for that frame.*/
	void update() { // This function is the "heart" of the EntityManager.
		// Add Pending Entities:	
		for (const auto& entity : m_entitiesToAdd) { // Loop through each entity in the "Waiting Room" vector (m_entitiesToAdd).
			m_entities.push_back(entity); // Add the entity to the main entity vector (m_entities).
			m_entitiesByTag[entity->getTag()].push_back(entity); // Also add the entity to the appropriate tag-based vector in the m_entitiesByTag map or create a new vector with a new tag.
		}
		m_entitiesToAdd.clear(); // Clear the "Waiting Room" vector after all pending entities have been added to the main entity vector and tag-based vectors.

		//Remove Inactive Entities:
		removeInactiveEntities(m_entities); // Call the helper function to remove inactive entities from the main entity vector (m_entities).
		for (auto& [tag, asscVec] : m_entitiesByTag) { // Loop through each entry in the m_entitiesByTag map, where 'tag' is the string tag and 'asscVec' is the associated vector.
			removeInactiveEntities(asscVec); // Call the helper function to remove inactive entities from each tag-based vector in the m_entitiesByTag map.
		}
	}
	
	std::shared_ptr<Entity> createEntity(const std::string& tag){ // Factory function to create a new entity with a specified tag.
		auto newEntity = std::shared_ptr<Entity>(new Entity(m_totalEntitiesCreated++, tag)); // Also increments the counter for the next entity to ensure unique IDs for each entity created.
		m_entitiesToAdd.push_back(newEntity); // Add the newly created entity to the "Waiting Room" vector (m_entitiesToAdd) to be added to the main entity and tag-based vectors.
		return newEntity; // Return the shared pointer to the newly created entity so the engine can assign components to it before it gest added to the main entity vector and tag-based vectors.
	}
	
	// Functions to retrieve entities from the EntityManager but read-only access.
	const EntityVector& getEntities() const{ // Function to return a const reference to the vector of all active entities in the game world.
		return m_entities; // Return a const reference to the main entity vector.
	}

	const EntityVector& getEntitiesByTag(const std::string& tag) const{ // Function to return a const reference to the vector of active entities that match a specific tag.
		static EntityVector vectorDoesNotExist; // Static vector to return if the specified tag does not exist in the m_entitiesByTag map. This avoids returning a reference to a temporary object.
		auto it = m_entitiesByTag.find(tag); // Search for the specified tag in the m_entitiesByTag map.
		if (it != m_entitiesByTag.end()) { // If the tag is found in the map,
			return it->second; // Return a const reference to the associated vector of entities for that tag.
		}
		else { // If the tag is not found in the map,
			return vectorDoesNotExist; // Return a reference to the static empty vector to indicate that there are no entities with that tag.
		}
	}
};