#include "EntityManager.hpp" // Include the header file that contains the declaration of the EntityManager class and its variables and functions.
#include <algorithm> // Include the algorithm library for using the std::remove_if function to remove inactive entities from vectors.

// Default constructor for the EntityManager class that initializes the entity manager and its data structures.
EntityManager::EntityManager() {
	// The constructor body is empty and there's no initializer list because everything is already handled by the header and default constructors.
}

void EntityManager::update() { // Function to update the EntityManager, called at the start of every frame to update the entity vectors.
	// Add Pending Entities:
	for (const auto& entity : m_entitiesToAdd) { // Loop through each entity in the "Waiting Room" vector (m_entitiesToAdd).
		m_entities.push_back(entity); // Add the entity to the main entity vector (m_entities).
		m_entitiesByTag[entity->getTag()].push_back(entity); // Also add the entity to the appropriate tag-based vector in the m_entitiesByTag map.
		//If the tag doesn't exist yet, this will create a new entry in the map with the tag and a vector containing the entity.
	}
	m_entitiesToAdd.clear(); // Clear the "Waiting Room" vector after all pending entities have been added to the main entity vector and tag-based vectors.

	// Remove Inactive Entities:
	removeInactiveEntities(m_entities); // Call the helper function to remove inactive entities from the main entity vector (m_entities).
	for (auto& [tag, entityVec] : m_entitiesByTag) { // Loop through each entry in the m_entitiesByTag map, where 'tag' is the string tag and 'entityVec' is the associated vector.
		removeInactiveEntities(entityVec); // Call the helper function to remove inactive entities from each tag-based vector in the m_entitiesByTag map.
	}
}

std::shared_ptr<Entity> EntityManager::createEntity(const std::string& tag) { // Factory function to create a new entity with a specified tag.
	// Create a new Entity object using the private constructor, passing in the counter value of m_totalEntitiesCreated and the specified tag.
	auto newEntity = std::shared_ptr<Entity>(new Entity(m_totalEntitiesCreated++, tag)); // Also increments the counter for the next entity to ensure unique IDs for each entity created.
	m_entitiesToAdd.push_back(newEntity); // Add the newly created entity to the "Waiting Room" vector (m_entitiesToAdd) to be added to the main entity and tag-based vectors.
	return newEntity; // Return the shared pointer to the newly created entity so the engine can assign components to it before it gest added to the main entity vector and tag-based vectors.
}

const EntityVector& EntityManager::getEntities() const { // Function to return a const reference to the vector of all active entities in the game world.
	return m_entities; // Return a const reference to the main entity vector.
}

const EntityVector& EntityManager::getEntitiesByTag(const std::string& tag) const { // Function to return a const reference to the vector of active entities that match a specific tag.
	static EntityVector vectorDoesNotExist; // Create a static empty vector to return if the specified tag does not exist in the m_entitiesByTag map.
	auto it = m_entitiesByTag.find(tag); // Attempt to find the specified tag in the m_entitiesByTag map.
	if (it != m_entitiesByTag.end()) { // If the tag is found in the map,
		return it->second; // return a const reference to the associated vector of entities for that tag.
	}
	else { // Otherwise, if the tag is not found in the map,
		return vectorDoesNotExist; // return a const reference to the static empty vector, indicating that there are no entities with that tag.
	}
}

void EntityManager::removeInactiveEntities(EntityVector& entities) { // Private helper function to clean up inactive entities from a vector without repeating code.
	// Use the std::remove_if algorithm to move inactive entities to the end of the vector and get an iterator to the new logical end of the vector.
	auto newEnd = std::remove_if(entities.begin(), entities.end(), [](const auto& entity) { // Use a lambda function as the predicate for std::remove_if to check if an entity is inactive.
		return !entity->isActive(); // Return true if the entity is not active, which will mark it for removal.
		});
	entities.erase(newEnd, entities.end()); // Erase everything from the newEnd marker to the actual end of the vector, removing all the entities that were moved by std::remove_if.