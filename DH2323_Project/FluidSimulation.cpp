#include "FluidSimulation.h"

FluidSimulation::FluidSimulation(unsigned int size)
{
	this->meshImage.Init(1, 1, size, size);
}