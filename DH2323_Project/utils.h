#pragma once
#ifndef MY_UTILS_H
#define MY_UTILS_H

#define PI 3.14159265359f

#include <cmath>
#include "SFML/System.hpp"

namespace utils {
	static float lengthSqrt(sf::Vector2f vector)
	{
		return vector.x * vector.x + vector.y * vector.y;
	}

	static float length(sf::Vector2f vector)
	{
		return sqrt(lengthSqrt(vector));
	}
}

#endif