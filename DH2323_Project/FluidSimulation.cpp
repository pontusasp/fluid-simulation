#include "FluidSimulation.h"

//#define IX(i, j) (i + (N + 2) * j) // does not work wtf????
#define IX(i,j) ((i)+(N+2)*(j))

FluidSimulation::FluidSimulation(unsigned int size, float diffusion, float viscosity)
{
	this->meshImage.Init(1, 1, size, size);

	unsigned int N = (size + 2) * (size + 2);

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
	quadCoord.x++;
	quadCoord.y++;
	if (quadCoord.x < 1 || quadCoord.x > size || quadCoord.y < 1 || quadCoord.y > size) return;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		AddDensity(quadCoord.x, quadCoord.y, .25f);
		AddVelocity(quadCoord.x, quadCoord.y, .2f, .2f);
	}
}

void FluidSimulation::UpdateImage()
{
	int N = this->size;
	for (unsigned int x = 1; x <= N; x++)
		for (unsigned int y = 1; y <= N; y++)
		{
			float d = density[IX(x, y)] * 10;
			if (d < 0) d = 0;
			if (d > 1) d = 1;
			meshImage.setColor(sf::Vector2u(x-1, y-1), sf::Color(255, 0, 0, (int)(255 * d)));
		}
}

// fixed which array gets used
void FluidSimulation::AddDensity(unsigned int x, unsigned int y, float amount)
{
	int N = this->size;
	this->s[IX(x, y)] += amount;
}

// fixed which array gets used
void FluidSimulation::AddVelocity(unsigned int x, unsigned int y, float amountX, float amountY)
{
	int N = this->size;
	int index = IX(x, y);
	this->vx0[index] += amountX;
	this->vy0[index] += amountY;
}

void FluidSimulation::Step(float dt, unsigned int iterations)
{
	int N = this->size;
	/*Diffuse(Axis::xx, vx0, vx, visc, dt, iterations, this->size);
	Diffuse(Axis::yy, vy0, vy, visc, dt, iterations, this->size);

	Project(vx0, vy0, vx, vy, iterations, this->size);

	Advect(Axis::xx, vx, vx0, vx0, vy0, dt, this->size);
	Advect(Axis::yy, vy, vy0, vx0, vy0, dt, this->size);

	Project(vx, vy, vx0, vy0, iterations, this->size);

	Diffuse(Axis::none, s, density, diff, dt, iterations, this->size);
	Advect(Axis::none, density, s, vx, vy, dt, this->size);*/
	vel_step(N, vx, vy, vx0, vy0, visc, dt, iterations);
	dens_step(N, density, s, vx, vy, diff, dt, iterations);
}

// ported
void FluidSimulation::SetBounds(Axis axis, std::vector<float>& x, unsigned int N)
{
	int i;
	for (i = 1; i <= N; i++) {
		x[IX(0, i)] = axis == Axis::xx ? -x[IX(1, i)] : x[IX(1, i)];
		x[IX(N + 1, i)] = axis == Axis::xx ? -x[IX(N, i)] : x[IX(N, i)];
		x[IX(i, 0)] = axis == Axis::yy ? -x[IX(i, 1)] : x[IX(i, 1)];
		x[IX(i, N + 1)] = axis == Axis::yy ? -x[IX(i, N)] : x[IX(i, N)];
	}
	x[IX(0, 0)] = 0.5*(x[IX(1, 0)] + x[IX(0, 1)]);
	x[IX(0, N + 1)] = 0.5*(x[IX(1, N + 1)] + x[IX(0, N)]);
	x[IX(N + 1, 0)] = 0.5*(x[IX(N, 0)] + x[IX(N + 1, 1)]);
	x[IX(N + 1, N + 1)] = 0.5*(x[IX(N, N + 1)] + x[IX(N + 1, N)]);
}

// ported
void FluidSimulation::Diffuse(Axis axis, std::vector<float>& vec, std::vector<float>& vec0,
	float diff, float dt, unsigned int iterations, unsigned int N)
{
	int i, j, k;
	float a = dt * diff*N*N;
	for (k = 0; k < iterations; k++) {
		for (i = 1; i <= N; i++) {
			for (j = 1; j <= N; j++) {
				vec[IX(i, j)] = (vec0[IX(i, j)] + a * (vec[IX(i - 1, j)] + vec[IX(i + 1, j)] +
					vec[IX(i, j - 1)] + vec[IX(i, j + 1)])) / (1 + 4 * a);
			}
		}
		SetBounds(axis, vec, N);
	}
}

// ported
void FluidSimulation::Project(std::vector<float>& u, std::vector<float>& v, std::vector<float>& p,
	std::vector<float>& div, unsigned int iterations, unsigned int N)
{
	int i, j, k;
	float h;
	h = 1.0 / N;
	for (i = 1; i <= N; i++) {
		for (j = 1; j <= N; j++) {
			div[IX(i, j)] = -0.5*h*(u[IX(i + 1, j)] - u[IX(i - 1, j)] +
				v[IX(i, j + 1)] - v[IX(i, j - 1)]);
			p[IX(i, j)] = 0;
		}
	}
	SetBounds(Axis::none, div, N);
	SetBounds(Axis::none, p, N);
	for (k = 0; k < 20; k++) {
		for (i = 1; i <= N; i++) {
			for (j = 1; j <= N; j++) {
				p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] +
					p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4;
			}
		}
		SetBounds(Axis::none, p, N);
	}
	for (i = 1; i <= N; i++) {
		for (j = 1; j <= N; j++) {
			u[IX(i, j)] -= 0.5*(p[IX(i + 1, j)] - p[IX(i - 1, j)]) / h;
			v[IX(i, j)] -= 0.5*(p[IX(i, j + 1)] - p[IX(i, j - 1)]) / h;
		}
	}
	SetBounds(Axis::xx, u, N);	SetBounds(Axis::yy, v, N);
}

// ported
void FluidSimulation::Advect(Axis axis, std::vector<float>& d, std::vector<float>& d0,
	std::vector<float>& u, std::vector<float>& v, float dt, unsigned int N)
{
	int i, j, i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1, dt0;
	dt0 = dt * N;
	for (i = 1; i <= N; i++) {
		for (j = 1; j <= N; j++) {
			x = i - dt0 * u[IX(i, j)]; y = j - dt0 * v[IX(i, j)];
			if (x < 0.5) x = 0.5; if (x > N + 0.5) x = N + 0.5; i0 = (int)x; i1 = i0 + 1;
			if (y < 0.5) y = 0.5; if (y > N + 0.5) y = N + 0.5; j0 = (int)y; j1 = j0 + 1;
			s1 = x - i0; s0 = 1 - s1; t1 = y - j0; t0 = 1 - t1;
			d[IX(i, j)] = s0 * (t0*d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
				s1 * (t0*d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
		}
	}
	SetBounds(axis, d, N);
}

// ported
void FluidSimulation::add_source(int N, std::vector<float>& x, std::vector<float>& s, float dt)
{
	int i, size = (N + 2)*(N + 2);
	for (i = 0; i < size; i++) x[i] += dt * s[i];
}

// ported
void FluidSimulation::dens_step(int N, std::vector<float>& x, std::vector<float>& x0, std::vector<float>& u,
	std::vector<float>& v, float diff, float dt, unsigned int iterations)
{
	add_source(N, x, x0, dt);
	x0.swap(x);
	Diffuse(Axis::none, x, x0, diff, dt, iterations, N);
	x0.swap(x);
	Advect(Axis::none, x, x0, u, v, dt, N);
}

// ported
void FluidSimulation::vel_step(int N, std::vector<float>& u, std::vector<float>& v, std::vector<float>& u0,
	std::vector<float>& v0, float visc, float dt, unsigned int iterations)
{
	add_source(N, u, u0, dt);
	add_source(N, v, v0, dt);
	u0.swap(u);
	Diffuse(Axis::xx, u, u0, visc, dt, iterations, N);
	v0.swap(v);
	Diffuse(Axis::yy, v, v0, visc, dt, iterations, N);
	Project(u, v, u0, v0, iterations, N);
	u0.swap(u);
	v0.swap(v);
	Advect(Axis::xx, u, u0, u0, v0, dt, N);
	Advect(Axis::yy, v, v0, u0, v0, dt, N);
	Project(u, v, u0, v0, iterations, N);
}