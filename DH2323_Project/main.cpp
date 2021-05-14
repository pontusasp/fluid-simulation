#include <SFML/Graphics.hpp>
#include <stdio.h>

#include "MeshImage.h"
#include "FluidSimulation.h"

using namespace sf;

void handleKeyboard(RenderWindow& window, Event& event)
{
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code)
		{
		case sf::Keyboard::Escape:
			window.close();
			break;
		default:
			printf("Unbound key %d.\n", event.key.code);
		}
	}
}

void handleEvents(RenderWindow& window, Event& event)
{
	if (event.type == sf::Event::Closed)
		window.close();
	handleKeyboard(window, event);
}

int main()
{
	Vector2u resolution(800, 800);
	RenderWindow window(sf::VideoMode(resolution.x, resolution.y), "Fluid Simulation Project by Pontus Asp");

	FluidSimulation sim(100, 10.f, 1.f);
	sim.setScale(Vector2f(resolution.x, resolution.y));

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			handleEvents(window, event);
		}

		window.clear();
		window.draw(sim);
		window.display();
	}

	return 0;
}