#include "hexa.h"
#include <cmath>
#include <algorithm>
const double PI = 3.141592653589793238463;

Hexa::Hexa(int x, int y, int z) :xPos(x), yPos(y), zPos(z),mColor(' ')
{
}

Hexa::~Hexa()
{
}

int Hexa::getXPos()const
{
	return xPos;
}

int Hexa::getYPos()const
{
	return yPos;
}

int Hexa::getZPos()const
{
	return zPos;
}

char Hexa::getColor()const
{
	return mColor;
}

float Hexa::getRenderXPos(float r)const
{
	return this->getXPos() * r * sqrt(3) + this->getYPos() * r * 0.5 * sqrt(3);
}

float Hexa::getRenderYPos(float r)const
{
	return this->getYPos() * r * 1.5;
}

float Hexa::getVertexXPos(float r,int number)const
{
	double angle = 2 * PI * (number - 1) / 6;
	return this->getRenderXPos(r) + r * sin(angle);
}

float Hexa::getVertexYPos(float r,int number)const
{
	double angle = 2 * PI * (number - 1) / 6;
	return this->getRenderYPos(r) + r * cos(angle);
}

Hexa* Hexa::getNear(int index)const
{
	return mNear[index];
}

void Hexa::setColor(char color)
{
	mColor = color;
}

void Hexa::setNear(int index, Hexa* hexa)
{
	mNear[index] = hexa;
}

int Hexa::distanceToCenter()const
{
	return std::max(std::max(abs(xPos), abs(yPos)), abs(zPos));
}