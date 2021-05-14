#include "FluidSimulation.h"

#define IX(x, y) (x + this->size * y)

FluidSimulation::FluidSimulation(unsigned int size, float diffusion, float viscosity)
{
	this->meshImage.Init(1, 1, size, size);

	unsigned int N = size * size;

	this->size = size;
	this->diff = diffusion;
	this->visc = viscosity;

	this->s.resize(N, 0.f);
	this->density.resize(N, 0.f);

	this->vx.resize(N, 0.f);
	this->vy.resize(N, 0.f);

	this->vx0.resize(N, 0.f);
	this->vy0.resize(N, 0.f);
}

void FluidSimulation::HandleMouse(sf::Window& window)
{
	auto mouse = sf::Mouse::getPosition(window);
	sf::Vector2f position = getPosition();
	sf::Vector2f scale = getScale();
	sf::Vector2f mousePos((mouse.x - position.x) / scale.x, (mouse.y - position.y) / scale.y);
	sf::Vector2u quadCoord = meshImage.fromScreenSpace(mousePos);
	if (quadCoord.x < 0 || quadCoord.x > size || quadCoord.y < 0 || quadCoord.y > size) return;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		AddDensity(quadCoord.x, quadCoord.y, .1f);
	}
}

void FluidSimulation::UpdateImage()
{
	for (int x = 0; x < size; x++)
		for (int y = 0; y < size; y++)
		{
			float d = density[IX(x, y)];
			if (d < 0) d = 0;
			if (d > 1) d = 1;
			meshImage.setColor(sf::Vector2u(x, y), sf::Color(255, 0, 0, (int)(255 * d)));
		}
}

void FluidSimulation::AddDensity(unsigned int x, unsigned int y, float amount)
{
	this->density[IX(x, y)] += amount;
}

void FluidSimulation::AddVelocity(unsigned int x, unsigned int y, float amountX, float amountY)
{
	int index = IX(x, y);
	this->vx[index] += amountX;
	this->vy[index] += amountY;
}

void FluidSimulation::Step(float dt, unsigned int iterations)
{
	Diffuse(Axis::x, vx0, vx, visc, dt, iterations);
	Diffuse(Axis::y, vy0, vy, visc, dt, iterations);

	Project(vx0, vy0, vx, vy, iterations);

	Advect(Axis::x, vx, vx0, vx0, vy0, dt);
	Advect(Axis::y, vy, vy0, vx0, vy0, dt);

	Project(vx, vy, vx0, vy0, iterations);

	Diffuse(Axis::none, s, density, diff, dt, iterations);
	Advect(Axis::none, density, s, vx, vy, dt);
}

void FluidSimulation::SetBounds(Axis axis, std::vector<float>& vec)
{
	
}

void FluidSimulation::LinearSolve(Axis axis, std::vector<float>& vec, std::vector<float>& vec0, float a, float c, unsigned int iterations)
{

}

void FluidSimulation::Diffuse(Axis axis, std::vector<float>& vec, std::vector<float>& vec0, float diff, float dt, unsigned int iterations)
{

}

void FluidSimulation::Project(std::vector<float>& vx, std::vector<float>& vy, std::vector<float>& p, std::vector<float>& div, unsigned int iterations)
{

}

void FluidSimulation::Advect(Axis axis, std::vector<float>& vec, std::vector<float>& vec0, std::vector<float>& vx, std::vector<float>& vy, float dt)
{

}