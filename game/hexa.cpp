#include "hexa.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include "../application/Application.h"

const double PI = 3.141592653589793238463;
const double epsilon = 0.000001;
const double ScreenRatio = 16.0 / 9.0;

Hexa::Hexa(int x, int y, int z) :xPos(x), yPos(y), zPos(z), mColor(' ')
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

float Hexa::getRenderXPos(float r)const
{
	return this->getXPos() * r * sqrt(3) + this->getYPos() * r * 0.5 * sqrt(3);
}

float Hexa::getRenderYPos(float r)const
{
	return this->getYPos() * r * 1.5;
}

float Hexa::getVertexXPos(float r, int number)const
{
	double angle = 2 * PI * number / 6;
	return this->getRenderXPos(r) + r * sin(angle);
}

float Hexa::getVertexYPos(float r, int number)const
{
	double angle = 2 * PI * number / 6;
	return this->getRenderYPos(r) + r * cos(angle);
}

bool Hexa::ifPositionInHexa(float x, float y, float r)const
{
	if (abs(y - this->getVertexYPos(r, 1) * ScreenRatio) < epsilon && this->getVertexXPos(r, -1) < x && x < this->getVertexXPos(r, 1))
		return true;
	if (abs(y - this->getVertexYPos(r, 2) * ScreenRatio) < epsilon && this->getVertexXPos(r, -2) < x && x < this->getVertexXPos(r, 2))
		return true;
	int count = 0;
	for (int i = 0; i < 6; i++)
	{
		float vertexX1 = this->getVertexXPos(r, i);
		float vertexX2 = this->getVertexXPos(r, i + 1);
		float vertexY1 = this->getVertexYPos(r, i) * ScreenRatio;
		float vertexY2 = this->getVertexYPos(r, i + 1) * ScreenRatio;

		float crossProductZ = (vertexX1 - vertexX2) * (vertexY1 - y) - (vertexY1 - vertexY2) * (vertexX1 - x);
		if (abs(crossProductZ) < epsilon)
			return false;
		if ((y > vertexY1 - epsilon && y > vertexY2 - epsilon) || (y < vertexY1 + epsilon && y < vertexY2 + epsilon))
			continue;
		if (vertexX1 == vertexX2)
		{
			if (vertexX1 > x)
				count++;
		}
		else
		{
			float k = (vertexX1 - vertexX2) / (vertexY1 - vertexY2);
			float r = k * (y - vertexY1) + vertexX1;
			if (k * (y - vertexY1) + vertexX1 > x)
				count++;
		}
	}
	if (count % 2 == 1)
		return true;
	return false;
}
bool Hexa::ifPositionInHexa(int x, int y, float r)const
{
	float fx = ((float)x - (float)(aplct->getWidth() / 2)) / (float)(aplct->getWidth() / 2);
	float fy = ((float)y - (float)(aplct->getLength() / 2)) / (float)(aplct->getLength() / 2);
	return ifPositionInHexa(fx, fy, r);
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