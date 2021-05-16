#include "ToggleButton.h"
#include <stdio.h>

ToggleButton::ToggleButton(bool * toggle, std::string text, sf::Vector2f location, sf::Vector2f size,
	sf::Color colorTrue, sf::Color colorFalse, sf::Color colorText, sf::Color colorOutline, sf::Font& font)
{
	setPosition(location);
	shape.setSize(size);
	shape.setFillColor(*toggle ? colorTrue : colorFalse);
	shape.setOutlineColor(colorOutline);
	shape.setOutlineThickness(2);

	textShape.setFont(font);
	textShape.setString(text);
	float charSize = textShape.getLetterSpacing();
	textShape.setCharacterSize(size.y);
	textShape.setFillColor(colorText);
	textShape.setPosition(sf::Vector2f(charSize*4, -size.y/4));

	this->toggle = toggle;
	this->text = text;
	this->colorTrue = colorTrue;
	this->colorFalse = colorFalse;
	this->colorText = colorText;
	this->colorOutline = colorOutline;
}

void ToggleButton::Update(sf::Window& window)
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !justPressed)
	{
		justPressed = true;
		sf::Vector2f pos = getPosition();
		sf::Vector2f size = shape.getSize();
		auto mouse = sf::Mouse::getPosition(window);
		if (mouse.x > pos.x && mouse.x < pos.x + size.x &&
			mouse.y > pos.y && mouse.y < pos.y + size.y)
		{
			*toggle = !*toggle;
			printf("Button \"%s\" clicked.\n", text);
		}
	}
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) justPressed = false;
	shape.setFillColor(*toggle ? colorTrue : colorFalse);
}