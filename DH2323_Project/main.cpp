#include <SFML/Graphics.hpp>
#include <stdio.h>

#include "MeshImage.h"

using namespace sf;

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 800), "Fluid Simulation Project by Pontus Asp");

	MeshImage meshImage(800, 800, 20, 20);

	bool mouseLeftDown = false;
	bool mouseRightDown = false;
	Vector2f mouseCoord;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code)
				{
				case 36: // ESC
					window.close();
				default:
					printf("Unbound key %d.\n", event.key.code);
				}
			}
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				mouseLeftDown = true;
			}
			if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
				mouseLeftDown = false;
			}
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
				mouseRightDown = true;
			}
			if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
				mouseRightDown = false;
			}
			if (event.type == sf::Event::MouseMoved) {
				mouseCoord.x = event.mouseMove.x;
				mouseCoord.y = event.mouseMove.y;
			}
		}

		if (mouseLeftDown) {
			meshImage.paintColor(meshImage.fromScreenSpacef(mouseCoord), sf::Color(255, 0, 0, 50));
		}

		if (mouseRightDown) {
			meshImage.setColor(meshImage.fromScreenSpace(mouseCoord), sf::Color::Blue);
		}

		window.clear();
		window.draw(meshImage);
		window.display();
	}

	return 0;
}