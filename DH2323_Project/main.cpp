#include <SFML/Graphics.hpp>
#include <stdio.h>

using namespace sf;

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 800), "Fluid Simulation Project by Pontus Asp");

	VertexArray triangle(Triangles, 3);

	triangle[0].position = Vector2f(10.f, 10.f);
	triangle[1].position = Vector2f(100.f, 10.f);
	triangle[2].position = Vector2f(100.f, 100.f);

	triangle[0].color = Color::Red;
	triangle[1].color = Color::Blue;
	triangle[2].color = Color::Green;

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
		}

		window.clear();
		window.draw(triangle);
		window.display();
	}

	return 0;
}