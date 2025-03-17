#include "hexa.h"
#include <cmath>
#include <algorithm>

Hexa::Hexa(int x, int y, int z) :xPos(x), yPos(y), zPos(z)
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