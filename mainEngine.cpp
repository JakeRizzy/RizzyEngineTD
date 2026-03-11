/*
Product Owner: Jacob Risoldi
Product Name: RizzyEngine
Version: 0.2.5
Version Date: March 2026
Language: C++ 20
Toolchain: SFML 2.6.1, ImGui 1.89.9, ImGui-SFML 2.6
*/

#include <iostream>         // Include the standard input-output stream library.
#include <vector>           // Include the standard vector library.
#include "SFML/Graphics.hpp" // Include the SFML Graphics library.
#include "imgui.h"          // Include the core ImGui library.
#include "imgui-SFML.h"    // Include the ImGui-SFML binding library.

int selectedIndex = 0; // Variable to hold the index of the selected shape, looks at the first shape by default.
sf::Font myFont; // Create a font object called "myFont".

struct Shapes { // Structure to hold shapes and their properties.
	sf::CircleShape circle; // Create a circle shape object called "circle"
	sf::RectangleShape rectangle; // Create a rectangle shape object called "rectangle".
	sf::Text shapeLabel; // For the shape's label in the window.
	std::string shapeName = "name"; // For the shape's name in ImGui.
	float velocityX = 0.00f, velocityY = 0.00f; // Initializing variables for movement along x and y axis.
	bool isCircle = true; // Initializing flag to determine if the shape is a circle or rectangle.
	bool isVisible = true; // Initializing flag to determine if the shape is visible.
	sf::Color color = sf::Color::White; // Initializing variable for shape color with a default of white.
};

void addCircle(std::vector<Shapes>& shapesContainer, sf::Font& myFont, std::string shapeName, float radius, 
float positionX, float positionY, float velocityX, float velocityY, bool isCircle, sf::Color color) { // Function to add a circle shape to the Shapes vector with variables for its properties.
	Shapes newShape; // Create a new Shapes object called "newShape".
	newShape.isCircle = true; // Flag the shape type as circle.
	newShape.circle.setRadius(radius); // Set the circle's radius.
	newShape.circle.setOrigin(radius, radius); // Set the circle's origin to be its center.
	newShape.circle.setPosition(positionX, positionY); // Set the circle's position along the x and y axis.
	newShape.circle.setFillColor(color); // Set the circle's color to the specified color.
	newShape.shapeName = shapeName; // Assign the new shape's name.
	
	newShape.shapeLabel.setFont(myFont); // Set the shape label's font.
	newShape.shapeLabel.setString(shapeName); // Set the shape label's string to the shape's name.
	newShape.shapeLabel.setCharacterSize(10); // Set the shape label's character size to 20.
	newShape.shapeLabel.setFillColor(sf::Color::Black); // Set the shape label's color to black.
	
	newShape.velocityX = velocityX; // Assign the new shape's velocity along the x axis.
	newShape.velocityY = velocityY; // Assign the new shape's velocity along the y axis.

	sf::FloatRect textBounds = newShape.shapeLabel.getLocalBounds(); // Get the local bounds of the shape label.
	newShape.shapeLabel.setOrigin(textBounds.left + textBounds.width / 2.00f, textBounds.top + textBounds.height / 2.00f); // Set the shape label's origin to the center of the shape label.

	shapesContainer.push_back(newShape); // Add the new shape to the shapesContainer vector.
}

void addRectangle(std::vector<Shapes>& shapesContainer, sf::Font& myFont, std::string shapeName, float width, float height,
	float positionX, float positionY, float velocityX, float velocityY, bool isCircle, sf::Color color) { // Function to add a rectangle shape to the Shapes vector with variables for its properties.
	Shapes newShape; // Create a new Shapes object called "newShape".
	newShape.isCircle = false; // Flag the shape type as rectangle.
	newShape.rectangle.setSize(sf::Vector2f(width, height)); // Set the rectangle's radius.
	newShape.rectangle.setOrigin(width / 2.00f, height / 2.00f); // Set the rectangle's origin to be its center.
	newShape.rectangle.setPosition(positionX, positionY); // Set the rectangle's position along the x and y axis.
	newShape.rectangle.setFillColor(color); // Set the rectangle's default color to white.
	newShape.shapeName = shapeName; // Assign the new shape's name.

	newShape.shapeLabel.setFont(myFont); // Set the shape label's font.
	newShape.shapeLabel.setString(shapeName); // Set the shape label's string to the shape's name.
	newShape.shapeLabel.setCharacterSize(10); // Set the shape label's character size to 25.
	newShape.shapeLabel.setFillColor(sf::Color::Black); // Set the shape label's color to black.

	newShape.velocityX = velocityX; // Assign the new shape's velocity along the x axis.
	newShape.velocityY = velocityY; // Assign the new shape's velocity along the y axis.

	sf::FloatRect textBounds = newShape.shapeLabel.getLocalBounds(); // Get the local bounds of the shape label.
	newShape.shapeLabel.setOrigin(textBounds.left + textBounds.width / 2.00f, textBounds.top + textBounds.height / 2.00f); // Set the shape label's origin to the center of the shape label.

	shapesContainer.push_back(newShape); // Add the new shape to the shapesContainer vector.
}

void selectShape(std::vector<Shapes>& shapesContainer, size_t index, int& selectedIndex) { // Function to select a shape from the shapesContainer vector based on its index.
	if (index < shapesContainer.size()) { // Check if the index is valid.
		selectedIndex = index; // Set the selectedIndex to the specified index.
	}
	else { // otherwise,
		std::cout << "Error index: " << index << " is out of bounds!" << std::endl; // Output an error message if the index is invalid.
	}
}

void removeSelectedShape(std::vector<Shapes>& shapesContainer, int& selectedIndex) { // Function to remove the selected shape from the shapesContainer vector.
	if (selectedIndex >= 0 && selectedIndex < shapesContainer.size()) { // Check if the selectedIndex is valid.
		shapesContainer.erase(shapesContainer.begin() + selectedIndex); // Remove the shape at the selectedIndex from the shapesContainer vector.
		selectedIndex = shapesContainer.empty() ? -1 : 0; // Reset selectedIndex to -1 if the container is empty, otherwise set it to 0.
	}
	else { // otherwise,
		std::cout << "Error index: " << selectedIndex << " is out of bounds!" << std::endl; // Output an error message if the selectedIndex is invalid.
	}
}

void modifySelectedColor(std::vector<Shapes>& shapesContainer, int selectedIndex, int r, int g, int b) { // Function to modify the color of the selected shape in the shapesContainer vector.
	if (selectedIndex < 0 || selectedIndex >= shapesContainer.size()) { // Check if the selectedIndex is invalid.
		return; // if it is invalid, exit the function without making any changes.
	}
	Shapes& selectedShape = shapesContainer[selectedIndex]; // Get a reference to the selected shape.
	selectedShape.color = sf::Color(r, g, b); // Create a new color from the provided RGB values and assign it to the shape's color property.
	if (selectedShape.isCircle) { // If the shape is a circle -
		selectedShape.circle.setFillColor(selectedShape.color); // Set the circle's color to the new color.
	}
	else { // Otherwise, if the shape is a rectangle -
		selectedShape.rectangle.setFillColor(selectedShape.color); // Set the rectangle's color to the new color.
	}
}

void modifySelectedSize(std::vector<Shapes>& shapesContainer, int selectedIndex, float newSizeX, float newSizeY) { // Function to modify the size of the selected shape in the shapesContainer vector.
	if (selectedIndex < 0 || selectedIndex >= shapesContainer.size()) { // Check if the selectedIndex is invalid.
		return; // if it is invalid, exit the function without making any changes.
	}
	Shapes& selectedShape = shapesContainer[selectedIndex]; // Get a reference to the selected shape.
	if (selectedShape.isCircle) { // If the shape is a circle -
		float currentRadius = selectedShape.circle.getRadius(); // Get the current radius of the circle.
		selectedShape.circle.setRadius(newSizeX); // Set the circle's radius to the new x value.
		selectedShape.circle.setOrigin(newSizeX, newSizeX); // Update the circle's origin to be its new center.
	}
	else { // Otherwise, if the shape is a rectangle -
		sf::Vector2f currentSize = selectedShape.rectangle.getSize(); // Get the current size of the rectangle.
		sf::Vector2f newSize(newSizeX, newSizeY); // Set the new x and y values for size.
		selectedShape.rectangle.setSize(sf::Vector2f (newSize)); // Set the rectangle's size to the new size.
		selectedShape.rectangle.setOrigin(newSizeX / 2.00f, newSizeY / 2.00f); // Update the rectangle's origin to be its new center.
	}
}

void modifySelectedSpeed(std::vector<Shapes>& shapesContainer, int selectedIndex, float newVelocityX, float newVelocityY) { // Function to modify the speed of the selected shape in the shapesContainer vector.
	if (selectedIndex < 0 || selectedIndex >= shapesContainer.size()) { // Check if the selectedIndex is invalid.
		return; // if it is invalid, exit the function without making any changes.
	}
	Shapes& selectedShape = shapesContainer[selectedIndex]; // Get a reference to the selected shape.
	float currentVelocityX = selectedShape.velocityX; // Get the current velocity along the x axis.
	float currentVelocityY = selectedShape.velocityY; // Get the current velocity along the y axis.
	selectedShape.velocityX = newVelocityX; // Set the shape's velocity along the x axis to the new velocity.
	selectedShape.velocityY = newVelocityY; // Set the shape's velocity along the y axis to the new velocity.
}

void modifySelectedName(std::vector<Shapes>& shapesContainer, int selectedIndex, std::string newName) { // Function to modify the name of the selected shape in the shapesContainer vector.
	if (selectedIndex < 0 || selectedIndex >= shapesContainer.size()) { // Check if the selectedIndex is invalid.
		return; // if it is invalid, exit the function without making any changes.
	}
	Shapes& selectedShape = shapesContainer[selectedIndex]; // Get a reference to the selected shape.
	selectedShape.shapeName = newName; // Set the shape's name to the new name.
	selectedShape.shapeLabel.setString(newName); // Update the shape label's string to the new name.
	sf::FloatRect textBounds = selectedShape.shapeLabel.getLocalBounds(); // Get the local bounds of the shape label.
	selectedShape.shapeLabel.setOrigin(textBounds.left + textBounds.width / 2.00f, textBounds.top + textBounds.height / 2.00f); // Set the shape label's origin to the center of the shape label.
}

void clearShapes(std::vector<Shapes>& shapesContainer) { // Function to clear all shapes from the shapesContainer vector.
	shapesContainer.clear(); // Clear all shapes from the shapesContainer vector.
}

int main() {
	if (!myFont.loadFromFile("Assets/Fonts/AstaSans-Regular.ttf")) { // Attempt to load a font file from the specified path.
		std::cout << "Could not find font file!" << std::endl; // If the font fails to load, output an error message.
	}

	std::vector<Shapes> shapesContainer; // Initialize a vector called "shapesContainer" to hold Shapes structures.
	sf::Clock deltaClock; // Create a clock object to track time between frames.

	addCircle(shapesContainer, myFont, "Circle 1", 50.00f, 100.00f, 100.00f, 50.00f, 50.00f, true, sf::Color(255, 0, 0)); // Add a circle shape to the shapesContainer vector.
	addRectangle(shapesContainer, myFont, "Rectangle 1", 120.00f, 80.00f, 400.00f, 300.00f, -50.00f, -50.00f, false, sf::Color(0, 0, 255)); // Add a rectangle shape to the shapesContainer vector.

	sf::RenderWindow window(sf::VideoMode(1280, 720), "RizzyEngine"); // Create a window with 1280x720 resolution.
	window.setFramerateLimit(60); // Set the frame rate to 60 FPS.
	ImGui::SFML::Init(window); // Initialize ImGui-SFML with the created window.	

	/* Text Test
	sf::Text textTest("Font loaded successfully", myFont, 24); // Create a text object, give it a message, assign the font, and set the character size.
	textTest.setFillColor(sf::Color::White); // Set the text color to white.
	textTest.setPosition(500.00f, 360.00f); // Set the text position right 500 and down 360 from the top left corner.
	*/

	while (window.isOpen()) { // The main game loop which continues until the window is closed.
/*Process Events*/
		sf::Event event; // Declares an event object to hold event data.
		while (window.pollEvent(event)) { // Poll for events every frame.
			ImGui::SFML::ProcessEvent(event); // Process events with/for ImGui.
			/*
			Code for handling different events goes here.
			*/
			if (event.type == sf::Event::Closed) { // If the "closed" event is triggered -
				window.close(); // then close the window.
			}
		}
/*Update Logic and States*/
		sf::Time deltaTime = deltaClock.restart(); // Get the time elapsed since the last frame and restart the clock for the next frame.
		float deltaTimeSeconds = deltaTime.asSeconds(); // Convert that elapsed time to a float representing seconds for ease of use and accuracy in movement calculations.
		ImGui::SFML::Update(window, deltaTime); // Pass/Update ImGui with the current window and delta time.

		for (Shapes& ns : shapesContainer) { // Loop through each shape in the shapesContainer vector.
			float newPositionX = (ns.velocityX * deltaTimeSeconds); // Calculate the change in position along the x axis based on velocity and delta time.
			float newPositionY = (ns.velocityY * deltaTimeSeconds); // Calculate the change in position along the y axis based on velocity and delta time.
			if (ns.isCircle) { // If the shape is a circle -
				ns.circle.move(newPositionX, newPositionY); // Move the circle by the calculated change in position.
			}
			else { // Otherwise, if the shape is a rectangle -
				ns.rectangle.move(newPositionX, newPositionY); // Move the rectangle by the calculated change in position.
			}

			sf::FloatRect shapeBounds = ns.isCircle ? ns.circle.getGlobalBounds() : ns.rectangle.getGlobalBounds(); // Get the global bounds of the shape based on the flag.
			if (shapeBounds.left <= 0.00f || (shapeBounds.left + shapeBounds.width) >= 1280.00f) { // If the shapes bounds hit or exceed the left or right window edges -
				ns.velocityX *= -1.00f; // Reverse the shape's velocity along the x axis.
			}
			if (shapeBounds.top <= 0.00f || (shapeBounds.top + shapeBounds.height) >= 720.00f){ // If the shapes bounds hit or exceed the top or bottom window edges -
				ns.velocityY *= -1.00f; // Reverse the shape's velocity along the y axis.
			}
			ns.shapeLabel.setPosition(shapeBounds.left + (shapeBounds.width / 2.00f), shapeBounds.top + (shapeBounds.height / 2.00f)); // Update the shape label's position to be centered on the shape.
		}

		ImGui::Begin("ShapesManager"); // Begin a new ImGui window titled "ShapesManager".
		// ImGui::Text("UI Text Test!"); // Display a text message test in the ImGui window.
		if (ImGui::BeginCombo("Shapes List", shapesContainer.empty() ? "None" : shapesContainer[selectedIndex].shapeName.c_str())) { // Create a drop down box to display the shapes.
			for (int i = 0; i < shapesContainer.size(); ++i) { // Loop through each shape in the shapesContainer vector.
				const bool isSelected = (selectedIndex == i); // Check if the current shape is selected
				if (ImGui::Selectable(shapesContainer[i].shapeName.c_str(), isSelected)) { // Creates a selectable option for the shapesName and when selected highlights it.
					selectedIndex = i; // Saves which item is selected.
				}
				if (isSelected) ImGui::SetItemDefaultFocus(); // Set the default focus item as the selected item.
			}
			ImGui::EndCombo(); // End list box properties.
		}
		
		ImGui::Separator(); // Create a visual separation in the ImGui window.
		
		if (ImGui::Button("Add Circle")) { // Create a button called "Add Circle".
			addCircle(shapesContainer, myFont, "Unnamed Circle", 50.00f, 100.00f, 100.00f, 50.00f, 50.00f, true, sf::Color(255, 255, 255)); // Add a circle shape to the shapesContainer vector when you click the button.
			selectedIndex = shapesContainer.size() - 1; // Select the new shape automatically.
		}
		ImGui::SameLine(); // Keep the next widget on the same line as the previous one.
		if (ImGui::Button("Add Rectangle")) { // Create a button called "Add Rectangle".
			addRectangle(shapesContainer, myFont, "Unnamed Rectangle", 120.00f, 80.00f, 400.00f, 300.00f, -50.00f, -50.00f, false, sf::Color(255, 255, 255)); // Add a rectangle shape to the shapesContainer vector.
			selectedIndex = shapesContainer.size() - 1; // Select the new shape automatically.
		}
		ImGui::SameLine(); // Keep the next widget on the same line as the previous one.
		if (ImGui::Button("Delete Selected Shape")) { // Create a button called "Delete Selected Shape".
			removeSelectedShape(shapesContainer, selectedIndex); // Call the funtion that deletes the shape from the vector.
		}
		ImGui::SameLine(); // Keep the next widget on the same line as the previous one.
		if (ImGui::Button("Delete ALL Shapes")) { // Create a button called "Delete ALL Shapes".
			clearShapes(shapesContainer); // Call the funtion that deletes ALL shapes from the vector.
		}

		ImGui::Separator(); // Create a visual separation in the ImGui window.

		if (!shapesContainer.empty() && selectedIndex >= 0 && selectedIndex < (int)shapesContainer.size()) { // Check if the shapesContainer vector is not empty and the selectedIndex is valid.
			Shapes& selectedShape = shapesContainer[selectedIndex]; // Get a reference to the selected shape.

			char buffer[255]; // Create a buffer to hold the shape name for editing because ImGui requires a char array.
			strncpy_s(buffer, selectedShape.shapeName.c_str(), sizeof(buffer)); // Copy the shape's name into the buffer safely.
			if (ImGui::InputText("Shape Name", buffer, sizeof(buffer))) { // Display an input text box for editing the shape's name.
				modifySelectedName(shapesContainer, selectedIndex, std::string(buffer)); // Call the function to modify the selected shape's name with the new name from the buffer.
			}

			ImGui::Separator(); // Create a visual separation in the ImGui window.

			float currentSize[2]; // Create an array to hold the current size values for editing.
			if (selectedShape.isCircle) { // If the selected shape is a circle -
				sf::Vector2f size(selectedShape.circle.getRadius(), selectedShape.circle.getRadius()); // Get the current size of the circle (radius for both x and y since circles are uniform).
				currentSize[0] = size.x; // Set the first element to the circle's radius.
				currentSize[1] = currentSize[0]; // Set the second element equal to the first element since circles x and y are uniform.
			}
			else { // Otherwise -
				sf::Vector2f size = selectedShape.rectangle.getSize(); // Get the current size of the rectangle.
				currentSize[0] = size.x; // Set the first element to the rectangle's width.
				currentSize[1] = size.y; // Set the second element to the rectangle's height.
			}
			if (ImGui::SliderFloat2("Adjust Size (X, Y)", currentSize, 1.00f, 500.00f)) { // Display 2 slider float boxes for editing the shape's size with a range of 1 to 500.
				modifySelectedSize(shapesContainer, selectedIndex, currentSize[0], currentSize[1]); // Call the function to modify the selected shape's size with the new size values from the sliders.
			}

			ImGui::Separator(); // Create a visual separation in the ImGui window.

			float currentSpeed[2]; // Create an array to hold the current x and y velocity values for editing.
			currentSpeed[0] = selectedShape.velocityX; // Set the first element to the shape's current velocity along the x axis.
			currentSpeed[1] = selectedShape.velocityY; // Set the second element to the shape's current velocity along the y axis.
			if (ImGui::SliderFloat2("Adjust Speed (X, Y)", currentSpeed, -500.00f, 500.00f)) { // Display 2 slider float boxes for editing the shape's speed with a range of -10 to 10.
				modifySelectedSpeed(shapesContainer, selectedIndex, currentSpeed[0], currentSpeed[1]); // Call the function to modify the selected shape's speed with the new velocity values from the sliders.
			}

			ImGui::Separator(); // Create a visual separation in the ImGui window.

			float colorValue[3] = { selectedShape.color.r / 255.00f, selectedShape.color.g / 255.00f, selectedShape.color.b / 255.00f }; // Create an array to hold the current color values for editing, and convert them from 0-255 (SFML) to 0.0-1.0 (ImGui).
			if (ImGui::ColorEdit3("Shape Color", colorValue)) { // Display a color editor for editing the shape's color.
				modifySelectedColor(shapesContainer, selectedIndex, colorValue[0] * 255, colorValue[1] * 255, colorValue[2] * 255); // Call the function to modify the selected shape's color with the new color values from the color editor, and convert them back from 0.0-1.0 (ImGui) to 0-255 (SFML).
			}
			ImGui::SameLine(); // Keep the next widget on the same line as the previous one.
			ImGui::Checkbox("Visible", &selectedShape.isVisible); // Create a checkbox to toggle the selected shape's visibility, which directly modifies the shape's isVisible property.
		}
		else { // If the shapesContainer vector is empty or the selectedIndex is invalid -
			ImGui::Text("List is empty! No Shapes, No Properties!"); // Display a message in the ImGui window indicating that there are no shapes to display or edit.
		}

		ImGui::End(); // End the ImGui window properties.
/*Render Frame*/
		window.clear(); // Clear the window with the default color (black).
		/*
		window.draw(textTest); // Draw the text object to confirm font loaded.
		*/
		for (Shapes& ns : shapesContainer) { // Loop through each shape in the shapesContainer vector.
			if (ns.isVisible) {
				if (ns.isCircle) { // If the shape is a circle -
					window.draw(ns.circle); // Draw the circle shape to the window.
				}
				else { // Otherwise, if the shape is a rectangle -
					window.draw(ns.rectangle); // Draw the rectangle shape to the window.
				}
			}
			window.draw(ns.shapeLabel); // Draw the shape's label to the window (after/on top of the shape).
		}
		ImGui::SFML::Render(window); // Draw the UI window last.
		window.display(); // Display the contents of the window.
	}
	ImGui::SFML::Shutdown(); // Shutdown ImGui-SFML to clean up resources.
	return 0; // End of program, successful execution.
}