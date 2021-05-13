#include "MeshImage.h"
#include <math.h>
#include "utils.h"
#include <stdio.h>

MeshImage::MeshImage(float width, float height, unsigned int resolutionX, unsigned int resolutionY)
{
	this->width = width;
	this->height = height;
	this->resolutionX = resolutionX;
	this->resolutionY = resolutionY;

	this->width_quad = width / resolutionX;
	this->height_quad = height / resolutionY;
	this->hyp_quad = utils::length(sf::Vector2f(width_quad, height_quad));

	// Set vertex type to quad
	m_vertices.setPrimitiveType(sf::Quads);
	// Allocate space for our quads
	m_vertices.resize(resolutionX * resolutionY * 4);

	for (unsigned int x = 0; x < resolutionX; x++)
		for (unsigned int y = 0; y < resolutionY; y++)
		{
			QuadPointer quad = get(sf::Vector2u(x, y));

			// set quad vertex positions
			quad.tl->position = sf::Vector2f((x + 0) * width_quad, (y + 0) * height_quad); // TL
			quad.tr->position = sf::Vector2f((x + 1) * width_quad, (y + 0) * height_quad); // TR
			quad.br->position = sf::Vector2f((x + 1) * width_quad, (y + 1) * height_quad); // BR
			quad.bl->position = sf::Vector2f((x + 0) * width_quad, (y + 1) * height_quad); // BL

			// define texture coordinates, could be used in shaders later.
			// uv coordinates goes from 0 - 1 across the entire Mesh Image.
			quad.tl->texCoords = sf::Vector2f(quad.tl->position.x / width, quad.tl->position.y / height);
			quad.tr->texCoords = sf::Vector2f(quad.tr->position.x / width, quad.tr->position.y / height);
			quad.br->texCoords = sf::Vector2f(quad.br->position.x / width, quad.br->position.y / height);
			quad.bl->texCoords = sf::Vector2f(quad.bl->position.x / width, quad.bl->position.y / height);
		}

}

QuadPointer MeshImage::get(sf::Vector2u coord)
{
	if (coord.x >= resolutionX || coord.y >= resolutionY) return QuadPointer();
	// get pointer to current quad
	sf::Vertex* quad = &m_vertices[(coord.x + coord.y * resolutionX) * 4];
	QuadPointer qp = {
		&quad[0],
		&quad[1],
		&quad[2],
		&quad[3],
	};
	return qp;
}

void MeshImage::setColor(sf::Vector2u coord, sf::Color color)
{
	QuadPointer quad = get(coord);
	if (quad.tl == nullptr) return;
	// Set color to all vertices in quad
	quad.tl->color = quad.tr->color = quad.br->color = quad.bl->color = color;
}

void MeshImage::paintColor(sf::Vector2f coord, sf::Color color)
{
	QuadPointer quads[] = {
		get(sf::Vector2u(coord.x - 1, coord.y - 1)), get(sf::Vector2u(coord.x, coord.y - 1)), get(sf::Vector2u(coord.x + 1, coord.y - 1)),
		get(sf::Vector2u(coord.x - 1, coord.y)), get(sf::Vector2u(coord)), get(sf::Vector2u(coord.x + 1, coord.y)),
		get(sf::Vector2u(coord.x - 1, coord.y + 1)), get(sf::Vector2u(coord.x, coord.y + 1)), get(sf::Vector2u(coord.x + 1, coord.y + 1)),
	};

	for (int i = 0; i < 9; i++) {
		QuadPointer quad = quads[i];
		if (quad.tl == nullptr) continue;
		sf::Vertex* vertices[] = { quad.tl, quad.tr, quad.br, quad.bl };
		for (int j = 0; j < 4; j++) {
			sf::Vertex* vertex = vertices[j];
			float dst = utils::length(fromScreenSpacef(vertex->position) - coord);
			float maxDst = sqrt(2) / 2;
			if (dst < maxDst) {
				float alpha = (maxDst - dst) / maxDst * (color.a / 255.f);
				vertex->color = sf::Color(
					vertex->color.r * (1 - alpha) + color.r * alpha,
					vertex->color.g * (1 - alpha) + color.g * alpha,
					vertex->color.b * (1 - alpha) + color.b * alpha
				);
			}
		}
	}
}

sf::Vector2f MeshImage::fromScreenSpacef(sf::Vector2f coord)
{
	return sf::Vector2f(coord.x / width_quad, coord.y / height_quad);
}

sf::Vector2u MeshImage::fromScreenSpace(sf::Vector2f coord)
{
	return sf::Vector2u(coord.x / width_quad, coord.y / height_quad);
}

void MeshImage::paintDebug()
{
	for (unsigned int x = 0; x < resolutionX; x++)
		for (unsigned int y = 0; y < resolutionY; y++)
		{
			// Select color for quad
			sf::Color color = pow(-1, x + y) < 0 ? sf::Color::Magenta : sf::Color::Red;
			setColor(sf::Vector2u(x, y), color);
		}
}