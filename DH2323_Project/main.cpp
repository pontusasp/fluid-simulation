#include <SFML/Graphics.hpp>
#include <stdio.h>

#include "MeshImage.h"
#include "FluidSimulation.h"
#include "VectorField.h"

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

	FluidSimulation sim(50, 0.00001f, 0.0000002f);
	sim.setScale(Vector2f(resolution.x, resolution.y));

	VectorField field;
	field.Init(800, 800, 5, 5);

	Vector2u pos(2, 2);
	field.setVector(pos, Vector2f(10, 10));

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			handleEvents(window, event);
		}
		sim.HandleMouse(window);
		sim.Step(.1f, 4);
		sim.UpdateImage();
		window.clear();
		window.draw(sim);
		//window.draw(field);
		window.display();
	}

	return 0;
}