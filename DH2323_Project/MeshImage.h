#pragma once

#include <SFML/Graphics.hpp>

class MeshImage : public sf::Drawable, public sf::Transformable
{
public:
	MeshImage(float width, float height, unsigned int resolutionX, unsigned int resolutionY);

private:
	float width, height;
	int resolutionX, resolutionY;

	sf::VertexArray m_vertices;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(m_vertices, states);
	}
};

