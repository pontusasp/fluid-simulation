#include "FluidSimulation.h"

//#define IX(i, j) (i + (N + 2) * j) // DOES NOT WORK, KEEP IN MIND IN FUTURE
#define IX(i, j) ((i) + (N + 2) * (j))

FluidSimulation::FluidSimulation(unsigned int size, float diffusion, float viscosity)
{
	this->meshImage.Init(1, 1, size, size);
	this->vectorField.Init(1, 1, size, size);

	unsigned int N = (size + 2) * (size + 2);

	this->size = size;
	this->diff = diffusion;
	this->visc = viscosity;

	this->density0.resize(N, 0.f);
	this->density.resize(N, 0.f);

	this->vx.resize(N, 0.f);
	this->vy.resize(N, 0.f);

	this->vx0.resize(N, 0.f);
	this->vy0.resize(N, 0.f);
}

void FluidSimulation::HandleMouse(sf::Window& window)
{
	static sf::Vector2f lastMousePos;
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
		AddVelocity(quadCoord.x, quadCoord.y, (mousePos.x - lastMousePos.x) * 100, (mousePos.y - lastMousePos.y) * 100);
	}
	lastMousePos = mousePos;
}

void FluidSimulation::UpdateImage()
{
	int N = this->size;
	for (unsigned int x = 1; x <= N; x++)
		for (unsigned int y = 1; y <= N; y++)
		{
			float d = density[IX(x, y)] * 10;
			if (d < 0) d = 0;

			int r = 255 * d;
			int g = 10 * d;
			int b = 20 * d * d;
			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;

			sf::Vector2u coord(x - 1, y - 1);
			meshImage.setColor(coord, sf::Color(r, g, b));
			vectorField.setVector(coord, sf::Vector2f(vx[IX(x, y)], vy[IX(x, y)]));
		}
}

void FluidSimulation::AddDensity(unsigned int x, unsigned int y, float amount)
{
	int N = this->size;
	this->density[IX(x, y)] += amount;
}

void FluidSimulation::AddVelocity(unsigned int x, unsigned int y, float amountX, float amountY)
{
	int N = this->size;
	int index = IX(x, y);
	this->vx[index] += amountX;
	this->vy[index] += amountY;
}

void FluidSimulation::Step(float dt, unsigned int iterations)
{
	Diffuse(Axis::xx, vx0, vx, visc, dt, iterations, this->size);
	Diffuse(Axis::yy, vy0, vy, visc, dt, iterations, this->size);

	Project(vx0, vy0, vx, vy, iterations, this->size);

	Advect(Axis::xx, vx, vx0, vx0, vy0, dt, this->size);
	Advect(Axis::yy, vy, vy0, vx0, vy0, dt, this->size);

	Project(vx, vy, vx0, vy0, iterations, this->size);

	Diffuse(Axis::none, density0, density, diff, dt, iterations, this->size);
	Advect(Axis::none, density, density0, vx, vy, dt, this->size);
}

void FluidSimulation::SetBounds(Axis axis, std::vector<float>& vec, unsigned int N)
{
	for (unsigned int i = 1; i <= N; i++)
	{
		vec[IX(0    , i)] = axis == Axis::xx ? -vec[IX(1, i)] : vec[IX(1, i)];
		vec[IX(N + 1, i)] = axis == Axis::xx ? -vec[IX(N, i)] : vec[IX(N, i)];
		vec[IX(i, 0    )] = axis == Axis::yy ? -vec[IX(i, 1)] : vec[IX(i, 1)];
		vec[IX(i, N + 1)] = axis == Axis::yy ? -vec[IX(i, N)] : vec[IX(i, N)];
	}
	vec[IX(0, 0)] = 0.5f * (vec[IX(1, 0)] + vec[IX(0, 1)]);
	vec[IX(0, N+1)] = 0.5f * (vec[IX(1, N+1)] + vec[IX(0, N)]);
	vec[IX(N+1, 0)] = 0.5f * (vec[IX(N, 0)] + vec[IX(N+1, 1)]);
	vec[IX(N+1, N+1)] = 0.5f * (vec[IX(N, N+1)] + vec[IX(N+1, N)]);
}

void FluidSimulation::LinearSolve(Axis axis, std::vector<float>& vec, std::vector<float>& vec0, float a, float c, unsigned int iterations, unsigned int N)
{
	for (unsigned int k = 0; k < iterations; k++)
	{
		for (unsigned int i = 1; i <= N; i++)
			for (unsigned int j = 1; j <= N; j++)
			{
				vec[IX(i, j)] = (vec0[IX(i, j)] + a * (vec[IX(i - 1, j)] + vec[IX(i + 1, j)] +
					vec[IX(i, j - 1)] + vec[IX(i, j + 1)])) / (1 + 4 * a);
			}
		SetBounds(axis, vec, N);
	}
}

void FluidSimulation::Diffuse(Axis axis, std::vector<float>& vec, std::vector<float>& vec0, float diff, float dt, unsigned int iterations, unsigned int N)
{
	float a = dt * diff * N * N;
	LinearSolve(axis, vec, vec0, a, 1 + 6 * a, iterations, N);
}

void FluidSimulation::Project(std::vector<float>& vx, std::vector<float>& vy, std::vector<float>& p, std::vector<float>& div, unsigned int iterations, unsigned int N)
{
	float h;
	h = 1.0f / N;
	for (unsigned int i = 1; i <= N; i++) {
		for (unsigned int j = 1; j <= N; j++) {
			div[IX(i, j)] = -0.5f*h*(vx[IX(i + 1, j)] - vx[IX(i - 1, j)] +
				vy[IX(i, j + 1)] - vy[IX(i, j - 1)]);
			p[IX(i, j)] = 0;
		}
	}
	SetBounds(Axis::none, div, N);
	SetBounds(Axis::none, p, N);
	for (unsigned int k = 0; k < iterations; k++) {
		for (unsigned int i = 1; i <= N; i++) {
			for (unsigned int j = 1; j <= N; j++) {
				p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] +
					p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4;
			}
		}
		SetBounds(Axis::none, p, N);
	}
	for (unsigned int i = 1; i <= N; i++) {
		for (unsigned int j = 1; j <= N; j++) {
			vx[IX(i, j)] -= 0.5f*(p[IX(i + 1, j)] - p[IX(i - 1, j)]) / h;
			vy[IX(i, j)] -= 0.5f*(p[IX(i, j + 1)] - p[IX(i, j - 1)]) / h;
		}
	}
	SetBounds(Axis::xx, vx, N);
	SetBounds(Axis::yy, vy, N);
}

void FluidSimulation::Advect(Axis axis, std::vector<float>& vec, std::vector<float>& vec0, std::vector<float>& vx, std::vector<float>& vy, float dt, unsigned int N)
{
	int i0, j0, i1, j1;
	float x, y, frac_x_inv, frac_y_inv, frac_x, frac_y, dt0;
	dt0 = dt * N;
	for (unsigned int i = 1; i <= N; i++) {
		for (unsigned int j = 1; j <= N; j++) {
			x = i - dt0 * vx[IX(i, j)];
			y = j - dt0 * vy[IX(i, j)];

			if (x < 0.5f) x = 0.5f;
			if (x > N + 0.5f) x = N + 0.5f;
			i0 = (int)x;
			i1 = i0 + 1;

			if (y < 0.5f) y = 0.5f;
			if (y > N + 0.5f) y = N + 0.5f;
			j0 = (int)y;
			j1 = j0 + 1;

			frac_x = x - i0; // extract decimal value from x
			frac_x_inv = 1 - frac_x;

			frac_y = y - j0; // extract decimal value from y
			frac_y_inv = 1 - frac_y;

			vec[IX(i, j)] =
				frac_x_inv * (
					frac_y_inv*vec0[IX(i0, j0)] +
					frac_y * vec0[IX(i0, j1)]
				) +
				frac_x * (
					frac_y_inv*vec0[IX(i1, j0)] +
					frac_y * vec0[IX(i1, j1)]
				);
		}
	}
	SetBounds(axis, vec, N);
}