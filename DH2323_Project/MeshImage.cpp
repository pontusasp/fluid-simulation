#include "MeshImage.h"

MeshImage::MeshImage(float width, float height, unsigned int resolutionX, unsigned int resolutionY)
{
	this->width = width;
	this->height = height;
	this->resolutionX = resolutionX;
	this->resolutionY = resolutionY;

	m_vertices.setPrimitiveType(sf::Quads);
	m_vertices.resize(resolutionX * resolutionY * 4);
}