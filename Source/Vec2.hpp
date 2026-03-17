#pragma once // Prevents multiple inclusions of this header file.
#include <cmath> // Include the cmath library for mathematical functions like sqrt.
#include <SFML/Graphics.hpp> // Include SFML to bridge sf::Vector2 with our custom Vec2 class.

template <typename T> // Initialize a template for the Vec2 class, allowing it to work with any numeric type (e.g., int, float, double, etc.).

struct Vec2 { // Defines the custom 2d vector class for the foundation of the engine's math system.
	T x = {}, y = {}; // Initialize x and y coordinates to default values (0 for numeric types) using uniform initialization.

	Vec2() {} // Default constructor for Vec2: creates a vector with default values (0, 0).
	Vec2(T xin, T yin) : x(xin), y(yin) {} // Parameterized constructor for Vec2: creates a vector with specified x and y values.
	Vec2(const sf::Vector2<T>& vec) : x(vec.x), y(vec.y) {} // Constructor that converts an sf::Vector2 to our custom Vec2 by passing the x and y values from the sf::Vector2 to our Vec2.

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

	float length() const { // Function to calculate the length (magnitude) of the vector.
		return std::sqrt(static_cast<float>((x * x) + (y * y))); // Return the square root of the sum of the squares of the x and y coordinates.
	}
	
	float distance(const Vec2 & target) const { // Function to calculate the distance between the current vector and another vector.
		T dx = target.x - x, dy = target.y - y; // Calculate the differences in x and y coordinates between the target vector and the current vector.
		return std::sqrt(static_cast<float>((dx * dx) + (dy * dy))); // Return the square root of the sum of the squares of the differences in x and y coordinates.
	}

	void normalize() { // Function to normalize the vector by modifying it to have a length of 1 while maintaining its direction.
		float len = length(); // Calculate the current length of the vector.
		if (len != 0.00f) { // Check if the length is not zero to avoid division by zero, if it is not...
			x = static_cast<T>(x / len), y = static_cast<T>(y / len); // Cast to T to ensure compatibility. Divide both the x and y coordinates by the length to normalize the vector.
		}
	}
};