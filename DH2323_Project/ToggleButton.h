#pragma once
#include <string>
#include <SFML/Graphics.hpp>

class ToggleButton : public sf::Drawable, public sf::Transformable
{
public:
	ToggleButton(bool* toggle, std::string text, sf::Vector2f location, sf::Vector2f size,
		sf::Color colorTrue, sf::Color colorFalse, sf::Color colorText, sf::Color colorOutline, sf::Font& font);
	void Update(sf::Window& window);
private:
	bool* toggle;
	sf::RectangleShape shape;
	std::string text;
	sf::Color colorTrue, colorFalse, colorText, colorOutline;
	sf::Text textShape;
	sf::Vector2f size;

	bool justPressed = false;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(shape, states);
		target.draw(textShape, states);
	}
};

