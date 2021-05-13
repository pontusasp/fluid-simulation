#pragma once

#include <SFML/Graphics.hpp>
#include "MeshImage.h"

class FluidSimulation : public sf::Drawable, public sf::Transformable
{
public:
	FluidSimulation(unsigned int size);

private:
	MeshImage meshImage;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(meshImage, states);
	}
};

