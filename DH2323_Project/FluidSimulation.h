#pragma once

#include <SFML/Graphics.hpp>
#include "MeshImage.h"
#include <vector>

class FluidSimulation : public sf::Drawable, public sf::Transformable
{
public:
	FluidSimulation(unsigned int size, float diffusion, float viscosity);

	void AddDensity(unsigned int x, unsigned int y, float amount);
	void AddVelocity(unsigned int x, unsigned int y, float amountX, float amountY);
	void Step(float dt, unsigned int iterations);
	void HandleMouse(sf::Window& window);
	void UpdateImage();

private:
	MeshImage meshImage;

	unsigned int size;
	float diff, visc;
	std::vector<float> s;
	std::vector<float> density;

	std::vector<float> vx;
	std::vector<float> vy;

	std::vector<float> vx0;
	std::vector<float> vy0;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(meshImage, states);
	}

	enum Axis {
		none, xx, yy
	};

	static void SetBounds(Axis axis, std::vector<float>& vec, unsigned int N);
	static void LinearSolve(Axis axis, std::vector<float>& vec, std::vector<float>& vec0, float a, float c, unsigned int iterations, unsigned int N);
	static void Diffuse(Axis axis, std::vector<float>& vec, std::vector<float>& vec0, float diff, float dt, unsigned int iterations, unsigned int N);
	static void Project(std::vector<float>& vx, std::vector<float>& vy, std::vector<float>& p, std::vector<float>& div, unsigned int iterations, unsigned int N);
	static void Advect(Axis axis, std::vector<float>& vec, std::vector<float>& vec0, std::vector<float>& vx, std::vector<float>& vy, float dt, unsigned int N);
	
	static void add_source(int N, std::vector<float>& x, std::vector<float>& s, float dt);
	static void dens_step(int N, std::vector<float>& x, std::vector<float>& x0, std::vector<float>& u, std::vector<float>& v, float diff, float dt, unsigned int iterations);
	static void vel_step(int N, std::vector<float>& u, std::vector<float>& v, std::vector<float>& u0, std::vector<float>& v0, float visc, float dt, unsigned int iterations);

};

