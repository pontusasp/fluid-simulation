#pragma once

#include <SFML/Graphics.hpp>

struct QuadPointer {
	sf::Vertex* tl; // top left
	sf::Vertex* tr; // top right
	sf::Vertex* br; // bottom right
	sf::Vertex* bl; // bottom left
};

class MeshImage : public sf::Drawable, public sf::Transformable
{
public:
	MeshImage(float width, float height, unsigned int resolutionX, unsigned int resolutionY);

	QuadPointer get(sf::Vector2u);

	// Sets color of entire quad
	void setColor(sf::Vector2u, sf::Color color);
	// Paints color to nearby vertices
	void paintColor(sf::Vector2f, sf::Color color);

	sf::Vector2f fromScreenSpacef(sf::Vector2f);
	sf::Vector2u fromScreenSpace(sf::Vector2f);

	void paintDebug();

private:
	float width, height;
	int resolutionX, resolutionY;
	float width_quad, height_quad, hyp_quad;

	sf::VertexArray m_vertices;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(m_vertices, states);
	}
};

