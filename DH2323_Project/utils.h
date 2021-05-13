#pragma once
#include <math.h>
#include "SFML/System.hpp"

namespace utils {
	float lengthSqrt(sf::Vector2f vector)
	{
		return vector.x * vector.x + vector.y * vector.y;
	}

	float length(sf::Vector2f vector)
	{
		return sqrt(lengthSqrt(vector));
	}
}