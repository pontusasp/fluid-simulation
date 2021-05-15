#pragma once

#include <SFML/Graphics.hpp>

class VectorFieldArrow : public sf::Drawable, public sf::Transformable
{
public:
	sf::Vector2f vector;
	sf::Vector2f screenPosition;
	sf::VertexArray m_vertices;

	void Init(sf::Vector2u coord, sf::Vector2f fieldSize, sf::Vector2u fieldRes);
	void setPosition(sf::Vector2u& coord, sf::Vector2f& fieldSize, sf::Vector2u& fieldRes);
	void setVector(sf::Vector2f vector, sf::Vector2f fieldSize);

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(m_vertices, states);
	}
};

class VectorField : public sf::Drawable, public sf::Transformable
{
public:
	void Init(float width, float height, unsigned int resolutionX, unsigned int resolutionY);

	// Sets vector at coordinate
	void setVector(sf::Vector2u& coord, sf::Vector2f vector);

	sf::Vector2u fromScreenSpace(sf::Vector2f);

private:
	sf::Vector2f fieldSize;
	int resolutionX, resolutionY;

	std::vector<VectorFieldArrow> arrows;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		for (int i = 0; i < arrows.size(); i++)
			target.draw(arrows[i], states);
	}
};

