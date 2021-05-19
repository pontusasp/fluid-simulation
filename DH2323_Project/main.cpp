#include <SFML/Graphics.hpp>
#include <stdio.h>

#include "MeshImage.h"
#include "FluidSimulation.h"
#include "VectorField.h"
#include "ToggleButton.h"
#include <string>

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
	Vector2u simRes(800, 800);
	Vector2u toolRes(simRes.x, 50);
	Vector2u resolution(simRes.x, simRes.y + toolRes.y);
	RenderWindow window(sf::VideoMode(resolution.x, resolution.y), "Fluid Simulation Project by Pontus Asp", sf::Style::Titlebar | sf::Style::Close);

	Color bgColor(30, 30, 32);

	FluidSimulation sim(200, .125, 0.00000001f, 0.00000002f);
	sim.setScale(Vector2f(simRes.x, simRes.y));

	sf::Font font;
	if (!font.loadFromFile("upheavtt.ttf"))
	{
		printf("Failed to load font!\n");
		return -1;
	}

	int buttons = 0;
	bool paused = false;
	float marginLeft = 12.5f;
	float bWidth = 145;

	RectangleShape uiBorder;
	float borderThickness = 2.f;
	uiBorder.setSize(Vector2f(toolRes.x - borderThickness * 2, toolRes.y - borderThickness * 2));
	uiBorder.setOutlineColor(Color(100, 100, 100));
	uiBorder.setFillColor(bgColor);
	uiBorder.setOutlineThickness(borderThickness);
	uiBorder.setPosition(borderThickness, simRes.y + borderThickness);


	ToggleButton vectorFieldToggle(&sim.vectorFieldActive, "vector field", Vector2f(marginLeft + (marginLeft + bWidth) * buttons++, 815), Vector2f(bWidth, 20),
		Color(0, 150, 0), Color(40, 40, 40), Color::White, Color::White, font);

	ToggleButton clearWallsToggle(&sim.shouldClearWalls, "clear walls", Vector2f(marginLeft + (marginLeft + bWidth) * buttons++, 815), Vector2f(bWidth, 20),
		Color(40, 40, 40), Color(150, 0, 160), Color::White, Color::White, font);

	ToggleButton resetToggle(&sim.shouldReset, "reset", Vector2f(marginLeft + (marginLeft + bWidth) * buttons++, 815), Vector2f(bWidth, 20),
		Color(40, 40, 40), Color(180, 0, 0), Color::White, Color::White, font);

	ToggleButton simulationPause(&paused, "pause", Vector2f(marginLeft + (marginLeft + bWidth) * buttons++, 815), Vector2f(bWidth, 20),
		Color(0, 0, 200), Color(40, 40, 40), Color::White, Color::White, font);

	bool nextStep = true;
	ToggleButton nextStepToggle(&nextStep, "timestep + 1", Vector2f(marginLeft + (marginLeft + bWidth) * buttons++, 815), Vector2f(bWidth, 20),
		Color(200, 80, 0), Color(200, 80, 0), Color::White, Color::White, font);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			handleEvents(window, event);
		}
		vectorFieldToggle.Update(window);
		simulationPause.Update(window);
		clearWallsToggle.Update(window);
		resetToggle.Update(window);
		if (paused)
			nextStepToggle.Update(window);

		if (!paused)
		{
			sim.Step(.1f, 5);
			nextStep = true;
		}
		else if (nextStep)
		{
			sim.Step(.1f, 5);
			nextStep = false;
		}
		sim.HandleMouse(window);
		sim.UpdateImage();

		window.clear(bgColor);

		window.draw(sim);
		window.draw(uiBorder);
		window.draw(vectorFieldToggle);
		window.draw(simulationPause);
		window.draw(clearWallsToggle);
		window.draw(resetToggle);
		if (paused)
			window.draw(nextStepToggle);

		window.display();
	}

	return 0;
}