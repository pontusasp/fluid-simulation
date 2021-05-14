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