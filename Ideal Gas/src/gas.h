#pragma once

class gas
{
public:

	float mass = 1;
	float Temp = 400;
	float R = 1, G = 1, B = 1;
	
	float x = 0;
	float y = 0;
	float velx = 0;
	float vely = 0;
	float size = 3;
	float v = 0;
	
	void p(float pos_x, float pos_y);
	void draw();
	void move(float t);
	void setColor(float red, float green, float blue);
	void setVel();
};
float getVavg();
float* getV();
void gravity(bool gravityOn);
void run(std::vector<gas>& bn);

