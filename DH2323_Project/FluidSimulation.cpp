#include "FluidSimulation.h"

#define IX(x, y) (x + (N + 2) * y)

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
		AddDensity(quadCoord.x, quadCoord.y, 100.25f);
		AddVelocity(quadCoord.x, quadCoord.y, .2f, .6f);
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
	Diffuse(Axis::x, vx0, vx, visc, dt, iterations, this->size);
	Diffuse(Axis::y, vy0, vy, visc, dt, iterations, this->size);

	Project(vx0, vy0, vx, vy, iterations, this->size);

	Advect(Axis::x, vx, vx0, vx0, vy0, dt, this->size);
	Advect(Axis::y, vy, vy0, vx0, vy0, dt, this->size);

	Project(vx, vy, vx0, vy0, iterations, this->size);

	Diffuse(Axis::none, s, density, diff, dt, iterations, this->size);
	Advect(Axis::none, density, s, vx, vy, dt, this->size);
}

// FIXED
void FluidSimulation::SetBounds(Axis axis, std::vector<float>& x, unsigned int N)
{
	for (int i = 1; i < N - 1; i++) {
		x[IX(i, 0)] = axis == Axis::y ? -x[IX(i, 1)] : x[IX(i, 1)];
		x[IX(i, N - 1)] = axis == Axis::y ? -x[IX(i, N - 2)] : x[IX(i, N - 2)];
	}
	for (int j = 1; j < N - 1; j++) {
		x[IX(0, j)] = axis == Axis::x ? -x[IX(1, j)] : x[IX(1, j)];
		x[IX(N - 1, j)] = axis == Axis::x ? -x[IX(N - 2, j)] : x[IX(N - 2, j)];
	}

	x[IX(0, 0)]     = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
	x[IX(0, N-1)]   = 0.5f * (x[IX(1, N-1)] + x[IX(0, N-2)]);
	x[IX(N-1, 0)]   = 0.5f * (x[IX(N-2, 0)] + x[IX(N-1, 1)]);
	x[IX(N-1, N-1)] = 0.5f * (x[IX(N-2, N-1)] + x[IX(N-1, N-2)]);
}

// FIXED
void FluidSimulation::LinearSolve(Axis axis, std::vector<float>& vec, std::vector<float>& vec0, float a, float c, unsigned int iterations, unsigned int N)
{
	float cRecip = 1.0 / c;
	for (int k = 0; k < iterations; k++) {
		for (int j = 1; j < N - 1; j++) {
			for (int i = 1; i < N - 1; i++) {
				vec[IX(i, j)] =
					(vec0[IX(i, j)]
						+ a * (vec[IX(i + 1, j)]
							+ vec[IX(i - 1, j)]
							+ vec[IX(i, j + 1)]
							+ vec[IX(i, j - 1)]
							)) * cRecip;
			}
		}
		SetBounds(axis, vec, N);
	}
}

// FIXED
void FluidSimulation::Diffuse(Axis axis, std::vector<float>& vec, std::vector<float>& vec0, float diff, float dt, unsigned int iterations, unsigned int N)
{
	float a = dt * diff * (N - 2) * (N - 2);
	LinearSolve(axis, vec, vec0, a, 1 + 6 * a, iterations, N);
}

// FIXED
void FluidSimulation::Project(std::vector<float>& vx, std::vector<float>& vy, std::vector<float>& p, std::vector<float>& div, unsigned int iterations, unsigned int N)
{
	for (int j = 1; j < N - 1; j++) {
		for (int i = 1; i < N - 1; i++) {
			div[IX(i, j)] = -0.5f*(
				vx[IX(i + 1, j)]
				- vx[IX(i - 1, j)]
				+ vy[IX(i, j + 1)]
				- vy[IX(i, j - 1)]
				) / N;
			p[IX(i, j)] = 0;
		}
	}
	SetBounds(Axis::none, div, N);
	SetBounds(Axis::none, p, N);
	LinearSolve(Axis::none, p, div, 1, 6, iterations, N);

	for (int j = 1; j < N - 1; j++) {
		for (int i = 1; i < N - 1; i++) {
			vx[IX(i, j, k)] -= 0.5f * (p[IX(i + 1, j)]
				- p[IX(i - 1, j)]) * N;
			vy[IX(i, j, k)] -= 0.5f * (p[IX(i, j + 1)]
				- p[IX(i, j - 1)]) * N;
		}
	}
	SetBounds(Axis::x, vx, N);
	SetBounds(Axis::y, vy, N);
}

// FIXED
void FluidSimulation::Advect(Axis axis, std::vector<float>& vec, std::vector<float>& vec0, std::vector<float>& vx, std::vector<float>& vy, float dt, unsigned int N)
{
	float i0, i1, j0, j1;

	float dtx = dt * (N - 2);
	float dty = dt * (N - 2);
	float dtz = dt * (N - 2);

	float s0, s1, t0, t1;
	float tmp1, tmp2, x, y;

	float Nfloat = N;
	float ifloat, jfloat;
	int i, j;

	for (j = 1, jfloat = 1; j < N - 1; j++, jfloat++) {
		for (i = 1, ifloat = 1; i < N - 1; i++, ifloat++) {
			tmp1 = dtx * vx[IX(i, j)];
			tmp2 = dty * vy[IX(i, j)];
			x = ifloat - tmp1;
			y = jfloat - tmp2;

			if (x < 0.5f) x = 0.5f;
			if (x > Nfloat + 0.5f) x = Nfloat + 0.5f;
			i0 = floorf(x);
			i1 = i0 + 1.0f;
			if (y < 0.5f) y = 0.5f;
			if (y > Nfloat + 0.5f) y = Nfloat + 0.5f;
			j0 = floorf(y);
			j1 = j0 + 1.0f;

			s1 = x - i0;
			s0 = 1.0f - s1;
			t1 = y - j0;
			t0 = 1.0f - t1;

			int i0i = i0;
			int i1i = i1;
			int j0i = j0;
			int j1i = j1;

			vec[IX(i, j)] =
				s0 * (t0 * vec0[IX(i0i, j0i)]) +
				(t1 * vec0[IX(i0i, j1i)]) +
				s1 * (t0 * vec0[IX(i1i, j0i)]) +
				(t1 * vec0[IX(i1i, j1i)]);
		}
	}
	SetBounds(axis, vec, N);
}