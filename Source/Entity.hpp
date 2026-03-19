#pragma once // Prevents multiple inclusions of this header file.
#include "Components.hpp" // Include the file that contains the component structures for entities.
#include <tuple> // Include the tuple library for using std::tuple to store multiple components in a single variable (ComponentTuple).
#include <string> // Include the string library for using std::string. Required for the tag property used for identifying and categorizing entities.

class EntityManager; // Forward declaration of the EntityManager class to allow it to be declared as a friend of the Entity class.

using ComponentTuple = std::tuple< // Define a type alias for a tuple that contains all the components that an entity can have.
	CTransform, // The CTransform component for transformation properties of entities.
	CShape, // The CShape component for shape properties of entities.
	CCollision, // The CCollision component for collision properties of entities.
	CLifespan, // The CLifespan component for lifespan properties of entities.
	CInput, // The CInput component for input properties of entities.
	// Optional Components based on feature needs for entities in the engine to be commented out if not needed:
	// CHealth, // The CHealth component for health properties of entities.
	// CDamage, // The CDamage component for damage properties of entities.
	CScoreValue, // The CScoreValue component for scoring properties of entities.
	CSpecialAbility // The CSpecialAbility component for special ability properties of entities.
>;

class Entity { // Defines the base class for all game objects in the RizzyEngine.
	private: // Private members that only the EntityManager will interact with.
		friend class EntityManager; // Declare the EntityManager class as a friend of the Entity class, allowing it to access private members of Entity for managing entities and their components.

		ComponentTuple m_components;
		std::string m_tag = "untagged"; // Initialize a string label used for identifying categorizing entities (i.e., "Player", "Enemy", "Projectile", etc.) with a default value of "untagged".
		size_t m_id = 0; // Initialize a unique identifier for each entity, which can be used for tracking and referencing entities, with a default value of 0.
		bool m_isActive = true; // Initialize a boolean flag to indicate whether the entity is active in the game world, with a default value of true.

		// Private constructor for the Entity class that takes in an id and tag to initialize the entity's properties.
		// This constructor is private to ensure that entities can only be created and managed through the EntityManager.
		Entity(const size_t id, const std::string& tag) : m_id(id), m_tag(tag) {}

	public: // Public members that the Systems (i.e. sRender, sCollision, etc.) will interact with.
		// Entity(const size_t id, const std::string& tag) : m_id(id), m_tag(tag) {} // ONLY PUBLIC FOR TESTING PURPOSES BEFORE EntityManager IS IMPLEMENTED.

		// Public helper functions for the Systems to interact with the entity's properties and components.
		bool isActive() const { // Function that returns whether the entity is active in the game world. True if active, false if not.
			return m_isActive; // Return the value of the m_isActive flag to indicate whether the entity is active or not.
		}
		const std::string& getTag() const { // Function that returns a const reference to the entity's tag string for identifying and categorizing the entity.
			return m_tag; // Return a const reference to the m_tag string, allowing read-only access to the entity's tag.
		}
		const size_t getId() const { // Function that returns the unique identifier of the entity.
			return m_id; // Return the value of the m_id variable, which is the unique identifier for the entity.
		}
		void destroy() { // Function to set m_isActive to false, effectively marking the entity for deletion by the EntityManager.
			m_isActive = false; // Set the m_isActive flag to false, indicating that the entity is no longer active and should be removed from the game world by the EntityManager.
		}

		template <typename T> // Template function to check if the entity has a specific component type T.
		bool has() const { // Function that returns whether the entity has a specific component type T. True if the component exists for this entity, false if it does not.
			return get<T>().exists; // Return the value of the "exists" variable of the specified component type T to indicate whether the entity has that component or not.
		}

		template <typename T, typename... TArgs> // Template function to add a specific component type T to the entity, with perfect forwarding of arguments for initializing the component's properties.
		T& add(TArgs&&... mArgs) { // Function to add a specific component type T to the entity, with perfect forwarding of arguments for initializing the component's properties.
			auto& component = get<T>(); // Get a reference to the specified component type T for this entity.
			component = T(std::forward<TArgs>(mArgs)...); // Construct the component in place using perfect forwarding of the provided arguments to initialize the component's properties.
			component.exists = true; // Set the "exists" variable of the component to true, indicating that this entity now has this component.
			return component; // Return a reference to the newly added component, allowing for further modification if needed.
		}

		template <typename T> // Template function to get a reference to a specific component type T of the entity.
		T& get() { // Function that returns a reference to the specified component type T of the entity, allowing access to the component's properties and functionality.
			return std::get<T>(m_components); // Return a reference to the specified component type T from the m_components tuple, allowing access to the component's properties and functionality.
		}

		template <typename T> // Template function to get a const reference to a specific component type T of the entity.
		const T& get() const { // Function that returns a const reference to the specified component type T of the entity, allowing read-only access to the component's properties and functionality.
			return std::get<T>(m_components); // Return a const reference to the specified component type T from the m_components tuple, allowing read-only access to the component's properties and functionality.
		}

		template <typename T> // Template function to remove a specific component type T from the entity by resetting it to its default-constructed state.
		void remove() { // Function to remove a specific component type T from the entity by resetting it to its default-constructed state, effectively removing it from the entity.
			get<T>() = T(); // Reset the specified component type T to its default-constructed state, effectively removing it from the entity.
		}
};