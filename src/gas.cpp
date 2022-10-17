#include<iostream>
#include<vector>
#include<GLFW/glfw3.h>
#include<gsl/gsl_randist.h>
#include "gas.h";

float t = 1;		//time step
float mass = 1;
float k = 0.0138;
float R = 0.00831;
float Vavg = 0;
float g = -0.0098;
float gravityOn = true;
float* avgm2 = new float[10000];
const gsl_rng_type* T = gsl_rng_default;
gsl_rng* r = gsl_rng_alloc(T);

double setSigma(float Temp, float mass)
{
	return sqrt(k * Temp / mass);
};

float getVavg()
{
	return Vavg;
}

float* getV()
{
	return avgm2;
}

void gravity(bool gravityOn)
{
	if (gravityOn == true)
	{
		g = -0.0098;
	}
	else 
		g = 0;
}

double* maxwell(gsl_rng* r, double sigma, float Temp) 
{	
	
	double vx = sqrt(8 * R * Temp / (3.14 * mass)) * 0 + gsl_ran_gaussian_ratio_method(r, sigma);
	double vy = sqrt(8 * R * Temp / (3.14 * mass)) * 0 + gsl_ran_gaussian_ratio_method(r, sigma);
	double v[2] = {vx, vy};
	return v;
}


void gas::p(float pos_x, float pos_y)
{
	srand(time(0));
	x = pos_x;
	y = pos_y;
	
	size = 2;
}
void gas::setVel()
{
	velx = float(maxwell(r, setSigma(Temp, mass), Temp)[0]);
	vely = float(maxwell(r, setSigma(Temp, mass), Temp)[1]);
	v = sqrt(velx * velx + vely * vely);
}
void gas::setColor(float red, float green, float blue)
{
	R = red;
	G = green;
	B = blue;
};

void gas::draw()
{
	glColor3f(R, G, B);
	glVertex2f(x, y);

}
void gas::move(float t)
{
	vely +=(g * t);
	x += velx * t;
	y += vely * t;
}


float avg(std::vector<float> data)
{
	float sum = 0;
	for (int i = 0; i < size(data); i++)
	{
		sum += data[i];
	}

	return (sum / size(data));
}

std::vector<float> avgm;

int f = 0;
void run(std::vector <gas>& bn)
{	
	
	int it = 0;
	for (gas& i : bn)
	{	
		//Boundary Collisions:
		if (i.x > 1050 - i.size) { i.x = 1050 - i.size; i.velx = -i.velx; }
		if (i.x < 350 + i.size) { i.x = 350 + i.size; i.velx = -i.velx; }
		if (i.y > 700 - i.size) { i.y = 700 - i.size; i.vely = -i.vely; }
		if (i.y < i.size) { i.y = i.size; i.vely = -i.vely; }

		avgm2[it] = i.v;

		i.move(t);
		it += 1;

		avgm.push_back(i.v);
	}

	Vavg = avg(avgm);
	f += 1;
	avgm.clear();
}


