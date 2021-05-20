#include "VectorField.h"
#include "utils.h"

#define IX(i, j) ((i) + (N) * (j)) // Helper macro to convert a 2d index to a 1d index

void VectorFieldArrow::Init(sf::Vector2u coord, sf::Vector2f fieldSize, sf::Vector2u fieldRes)
{
	float sizeX = fieldSize.x / fieldRes.x;
	float sizeY = fieldSize.y / fieldRes.y;

	m_vertices.setPrimitiveType(sf::TriangleStrip);
	m_vertices.resize(7); // 7 vertices are required to draw an arrow
	
	setOrigin(sf::Vector2f(sizeX/2, sizeY/2));
	setPosition(coord, fieldSize, fieldRes);
	setVector(sf::Vector2f(0, 0), fieldSize); // Initialize each arrow to display a (0,0) vector.
}

// Sets the position if the arrow in the grid
void VectorFieldArrow::setPosition(sf::Vector2u& coord, sf::Vector2f& fieldSize, sf::Vector2u& fieldRes)
{
	float sizeX = fieldSize.x / fieldRes.x;
	float sizeY = fieldSize.y / fieldRes.y;
	// Places each vertex to form an arrow. This should be moved to the Init function since it does not have to be set
	// every time a position is changed, but since I am just updating the code with comments after submission I will
	// keep it here.
	m_vertices[0].position = sf::Vector2f(0, (1.75f/3) * sizeY);
	m_vertices[1].position = sf::Vector2f(0, (1.25f/3) * sizeY);
	m_vertices[2].position = sf::Vector2f((2.f/3) * sizeX, (1.75f/3) * sizeY);
	m_vertices[3].position = sf::Vector2f((2.f/3) * sizeX, (1.25f/3) * sizeY);
	m_vertices[4].position = sf::Vector2f((3.f/3) * sizeX, (1.5f/3) * sizeY);
	m_vertices[5].position = sf::Vector2f((2.f/3) * sizeX, (.5f/3) * sizeY);
	m_vertices[6].position = sf::Vector2f((2.f/3) * sizeX, (2.5f/3) * sizeY);

	// Sets the position of the arrow to the corresponding coordinate in the field.
	sf::Transformable::setPosition(sf::Vector2f((coord.x + 0.5f) * sizeX, (coord.y + 0.5f) * sizeY));
}

// Sets the vector the arrow should visualize
void VectorFieldArrow::setVector(sf::Vector2f vector, sf::Vector2f fieldSize)
{
	float magnitude = utils::length(vector);
	float magSqrt = sqrt(sqrt(magnitude));

	// Calculate the color of the arrow depending on magnitude
	int strength = (int) (255.f * magnitude * 25);
	if (strength < 0) strength = 0;
	if (strength > 255) strength = 255;
	auto color = sf::Color(strength, strength, 255/3 - strength/3);
	for (int i = 0; i < 7; i++)
		m_vertices[i].color = color;

	setRotation(atan2(vector.y, vector.x) * 180 / PI); // get the angle of the vector and rotate the arrow accordingly
	setScale(sf::Vector2f(magSqrt + .2f, magSqrt + .2f)); // The scaling is not linear to the magnitude for improved visual effect
}

// Initialize the VectorField with it's arrows
void VectorField::Init(float width, float height, unsigned int resolutionX, unsigned int resolutionY)
{
	this->fieldSize.x = width;
	this->fieldSize.y = height;
	this->resolutionX = resolutionX;
	this->resolutionY = resolutionY;
	int N = resolutionX;
	sf::Vector2u fieldRes(resolutionX, resolutionY);
	arrows.resize(resolutionX * resolutionY);
	for (int x = 0; x < resolutionX; x++)
		for (int y = 0; y < resolutionY; y++)
			arrows[IX(x, y)].Init(sf::Vector2u(x, y), fieldSize, fieldRes);
}

// Sets the arrow to visualize the given vector at (coord.x, coord.y) 
void VectorField::setVector(sf::Vector2u& coord, sf::Vector2f vector)
{
	int N = resolutionX;
	int index = IX(coord.x, coord.y);
	if (index > 0 && index < arrows.size())
		arrows[index].setVector(vector, fieldSize);
}

// Converts a coordinate from screen space to a coordinate in the VectorField.
sf::Vector2u VectorField::fromScreenSpace(sf::Vector2f coord)
{
	return sf::Vector2u(coord.x / (fieldSize.x / resolutionX), coord.y / (fieldSize.y / resolutionY));
}