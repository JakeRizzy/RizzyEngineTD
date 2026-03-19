#pragma once // Prevents multiple inclusions of this header file.
#include <cmath> // Include the cmath library for mathematical functions like sqrt.
#include <SFML/Graphics.hpp> // Include SFML to bridge sf::Vector2 with our custom Vec2 class.

template <typename T> // Initialize a template for the Vec2 class, allowing it to work with any numeric type (e.g., int, float, double, etc.).

struct Vec2 { // Defines the custom 2d vector class for the foundation of the engine's math system.
	T x = 0, y = 0; // Initialize x and y coordinates to hold the values of the vector, with default values of 0.

	Vec2() = default; // Default constructor for Vec2: creates a vector with default x and y values of 0.
	Vec2(T xin, T yin) : x(xin), y(yin) {} // Parameterized constructor for Vec2: creates a vector with specified x and y values.
	Vec2(const sf::Vector2<T>& vec) : x(vec.x), y(vec.y) {} // Constructor that converts an sf::Vector2 to our custom Vec2 by passing the x and y values from the sf::Vector2 to our Vec2.
	operator sf::Vector2<T>() { return sf::Vector2<T>(x, y); } // Operator that converts our custom Vec2 to an sf::Vector2 by returning a new sf::Vector2 with the x and y values from our Vec2.

	bool operator == (const Vec2& rhs) const { // Overload the equality operator to check if two vectors are exactly the same.
		return (x == rhs.x) && (y == rhs.y); // Return true if both x and y coordinates are equal, otherwise return false.
	}

	bool operator != (const Vec2& rhs) const { // Overload the inequality operator to check if two vectors are not the same.
		return (x != rhs.x) || (y != rhs.y); // Return true if either x or y coordinates are not equal, otherwise return false.
	}

	Vec2 operator + (const Vec2& rhs) const { // Overload the addition operator to add two vectors together.
		return Vec2(x + rhs.x, y + rhs.y); // Return a new vector containing the sum of the x and y coordinates of both vectors.
	}

	Vec2 operator - (const Vec2& rhs) const { // Overload the subtraction operator to subtract one vector from another.
		return Vec2(x - rhs.x, y - rhs.y); // Return a new vector containing the difference of the x and y coordinates of both vectors.
	}

	Vec2 operator * (const T scalar) const { // Overload the multiplication operator to multiply a vector by a scalar value.
		return Vec2(x * scalar, y * scalar); // Return a new vector with both coordinates multiplied by the scalar value.
	}

	Vec2 operator / (const T scalar) const { // Overload the division operator to divide a vector by a scalar value.
		return Vec2(x / scalar, y / scalar); // Return a new vector with both coordinates divided by the scalar value.
	}

	void operator += (const Vec2& rhs) { // Overload the addition assignment operator to add another vector directly to the current vector; modifying it.
		x += rhs.x; // Add the right-hand side vector's x coordinate to the current vector's x coordinate.
		y += rhs.y; // Add the right-hand side vector's y coordinate to the current vector's y coordinate.
	}

	void operator -= (const Vec2& rhs) { // Overload the subtraction assignment operator to subtract another vector directly from the current vector; modifying it.
		x -= rhs.x; // Subtract the right-hand side vector's x coordinate from the current vector's x coordinate.
		y -= rhs.y; // Subtract the right-hand side vector's y coordinate from the current vector's y coordinate.
	}

	void operator *= (const T scalar) { // Overload the multiplication assignment operator to multiply the current vector by a scalar value; modifying it.
		x *= scalar; // Multiply the current vector's x coordinate by the scalar value.
		y *= scalar; // Multiply the current vector's y coordinate by the scalar value.
	}

	void operator /= (const T scalar) { // Overload the division assignment operator to divide the current vector by a scalar value; modifying it.
		x /= scalar; // Divide the current vector's x coordinate by the scalar value.
		y /= scalar; // Divide the current vector's y coordinate by the scalar value.
	}

	float lengthSq() const { // Function to calculate the length squared of the vector, which is more efficient than calculating the actual length when only a relative comparison is needed.
		return static_cast<float>((x * x) + (y * y)); // Return the sum of the squares of the x and y coordinates, cast to float for precision (length squared of the vector).
	}

	float length() const { // Function to calculate the length (magnitude) of the vector.
		return std::sqrt(lengthSq()); // Return the square root of the length squared, which is the actual length (magnitude) of the vector.
	}
	
	float distanceSq(const Vec2 & target) const { // Function to calculate the distance squared between the current vector and another vector, which is more efficient than calculating the actual distance when only a relative comparison is needed.
		T dx = target.x - x, dy = target.y - y; // Calculate the differences in x and y coordinates between the target vector and the current vector.
		return static_cast<float>((dx * dx) + (dy * dy)); // Return the sum of the squares of the differences in x and y coordinates, cast to float for precision (distance squared between the two vectors).
	}
	
	float distance(const Vec2 & target) const { // Function to calculate the distance between the current vector and another vector.
		return std::sqrt(distanceSq(target)); // Return the square root of the distance squared, which is the actual distance between the two vectors.
	}

	void normalize() { // Function to normalize the vector by modifying it to have a length of 1 while maintaining its direction.
		float len = length(); // Calculate the current length of the vector.
		if (len != 0.00f) { // Check if the length is not zero to avoid division by zero, if it is not...
			x = static_cast<T>(x / len), y = static_cast<T>(y / len); // Cast to T to ensure compatibility. Divide both the x and y coordinates by the length to normalize the vector.
		}
	}
};
using Vec2f = Vec2<float>; // Create a type alias for Vec2<float> to simplify usage and improve code readability when working with 2D vectors of float.
using Vec2i = Vec2<int>; // Create a type alias for Vec2<int> to simplify usage and improve code readability when working with 2D vectors of int.
using Vec2u = Vec2<unsigned int>; // Create a type alias for Vec2<unsigned int> to simplify usage and improve code readability when working with 2D vectors of unsigned int.