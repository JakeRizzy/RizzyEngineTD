#include "Entity.hpp" // Include the header file that contains the declaration of the Entity class and its variables and functions.

Entity::Entity(const size_t id, const std::string& tag) : m_id(id), m_tag(tag) { // Constructor for the Entity class that takes in an id and tag to initialize the entity's properties.
	/*The constructor body is empty because the initialization of the entity's properties is handled in the member initializer list
	and EntityManager is responsible for creating entities and assigning their components.*/
}

bool Entity::isActive() const { // Function that returns whether the entity is active in the game world. True if active, false if not.
	return m_isActive; // Return the value of the m_isActive flag to indicate whether the entity is active or not.
}

const std::string& Entity::getTag() const { // Function that returns a const reference to the entity's tag string for identifying and categorizing the entity.
	return m_tag; // Return a const reference to the m_tag string, allowing read-only access to the entity's tag.
}

const size_t Entity::getId() const { // Function that returns the unique identifier of the entity.
	return m_id; // Return the value of the m_id variable, which is the unique identifier for the entity.
}

void Entity::destroy() { // Function to set m_isActive to false, effectively marking the entity for deletion by the EntityManager.
	m_isActive = false; // Set the m_isActive flag to false, indicating that the entity is no longer active and should be removed from the game world by the EntityManager.
}