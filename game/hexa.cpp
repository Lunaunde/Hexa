#include "hexa.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include"../glframework/core.h"
#include "../application/Application.h"

const double PI = 3.141592653589793238463;
const double epsilon = 0.000001;
const double ScreenRatio = 16.0 / 9.0;

Hexa::Hexa(int x, int y, int z) :xPos(x), yPos(y), zPos(z), mColor(-1, -1, -1), mShowColor(-1, -1, -1), mScale(0), mStartTime(glfwGetTime())
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

Color Hexa::getColor()const
{
	return mColor;
}
Color Hexa::getShowColor()const
{
	if (mScale >= 0)
		mShowColor = mColor;
	return mShowColor;
}

Hexa* Hexa::getNear(int index)const
{
	return mNear[index];
}

float Hexa::getCenterXPos(float side)const
{
	return this->getXPos() * side * sqrt(3) + this->getYPos() * side * 0.5 * sqrt(3);
}
float Hexa::getCenterXPos(float side, float rotation)const
{
	return this->getCenterXPos(side) * sin(rotation) - this->getCenterYPos(side) * cos(rotation);
}
float Hexa::getCenterYPos(float side)const
{
	return this->getYPos() * side * 1.5;
}
float Hexa::getCenterYPos(float side, float rotation)const
{
	return this->getCenterYPos(side) * sin(rotation) + this->getCenterXPos(side) * cos(rotation);
}

float Hexa::getVertexXPos(float side, int number, float scale = 1)const
{
	double angle = 2 * PI * number / 6;
	return this->getCenterXPos(side) + side * scale * sin(angle) * abs(sin(mScale * PI * 0.5));
}
float Hexa::getVertexXPos(float side, int number, float scale, float rotation)const
{
	return this->getVertexXPos(side, number, scale) * sin(rotation) - this->getVertexYPos(side, number, scale) * cos(rotation);
}

float Hexa::getVertexYPos(float side, int number, float scale = 1)const
{
	double angle = 2 * PI * number / 6;
	return this->getCenterYPos(side) + side * scale * cos(angle) * abs(sin(mScale * PI * 0.5));
}
float Hexa::getVertexYPos(float side, int number, float scale, float rotation)const
{
	return this->getVertexYPos(side, number, scale) * sin(rotation) + this->getVertexXPos(side, number, scale) * cos(rotation);
}

bool Hexa::ifPositionInHexa(float x, float y, float side, float scale = 1, float rotation = 0)const
{
	//rotation = glfwGetTime();
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			if (i == j)
				continue;
			if (abs(this->getVertexYPos(side, i) - this->getVertexYPos(side, j)) < epsilon && abs(y - this->getVertexYPos(side, i) * ScreenRatio) < epsilon && (x - this->getVertexXPos(side, i) * (x - this->getVertexYPos(side, i) < 0)))
			{
				std::cout << 1 << std::endl;
				return true;
			}
		}
	}
	int count = 0;
	for (int i = 0; i < 6; i++)
	{
		float vertexX1 = this->getVertexXPos(side, i, scale, rotation);
		float vertexX2 = this->getVertexXPos(side, i + 1, scale, rotation);
		float vertexY1 = this->getVertexYPos(side, i, scale, rotation) * ScreenRatio;
		float vertexY2 = this->getVertexYPos(side, i + 1, scale, rotation) * ScreenRatio;

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
			float side = k * (y - vertexY1) + vertexX1;
			if (k * (y - vertexY1) + vertexX1 > x)
				count++;
		}
	}
	if (count % 2 == 1)
		return true;
	return false;
}
bool Hexa::ifPositionInHexa(int x, int y, float side)const
{
	float fx = ((float)x - (float)(aplct->getWidth() / 2)) / (float)(aplct->getWidth() / 2);
	float fy = ((float)y - (float)(aplct->getLength() / 2)) / (float)(aplct->getLength() / 2);
	return ifPositionInHexa(fx, fy, side);
}
bool Hexa::ifPositionInHexa(int x, int y, float side, float scale = 1, float rotation = 0)const
{
	float fx = ((float)x - (float)(aplct->getWidth() / 2)) / (float)(aplct->getWidth() / 2);
	float fy = ((float)y - (float)(aplct->getLength() / 2)) / (float)(aplct->getLength() / 2);
	return ifPositionInHexa(fx, fy, side, scale, rotation);
}

void Hexa::setColor(Color color)
{
	mColor = color;
}
void Hexa::setColor(int r, int g, int b)
{
	mColor = Color(r, g, b);
	mScale = -1;
	mStartTime = glfwGetTime();
}
void Hexa::setColor(float r, float g, float b)
{
	mColor = Color(r, g, b);
	mScale = -1;
	mStartTime = glfwGetTime();
}
void Hexa::changeColor(Color color)
{
	mColor = color;
	mScale = -1;
	mStartTime = glfwGetTime() + 1;
}

void Hexa::setNear(int index, Hexa* hexa)
{
	mNear[index] = hexa;
}

int Hexa::distanceToCenter()const
{
	return std::max(std::max(abs(xPos), abs(yPos)), abs(zPos));
}

void Hexa::mScaleAdd()
{
	if (mScale < 1)
		mScale = glfwGetTime() - mStartTime;
	else if (mScale > 1)
		mScale = 1;
}