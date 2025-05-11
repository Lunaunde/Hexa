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
			float vertexX1 = this->getVertexXPos(side, i, scale, rotation);
			float vertexX2 = this->getVertexXPos(side, i + 1, scale, rotation);
			float vertexY1 = this->getVertexYPos(side, i, scale, rotation) * ScreenRatio;
			float vertexY2 = this->getVertexYPos(side, i + 1, scale, rotation) * ScreenRatio;
			if (abs(vertexY1 - vertexY2) < epsilon && abs(y - vertexY1) < epsilon && (x - vertexX1) * (x - vertexX2) < 0)
			{
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

void Hexa::changeColor()
{
	if (mColor == Color::pureWhite)
		mColor = Color::pureBlack;
	else if (mColor == Color::pureBlack)
		mColor = Color::pureWhite;
	else if (mColor == Color::pureRed)
		mColor = Color::pureYellow;
	else if (mColor == Color::pureYellow)
		mColor = Color::pureBlue;
	else if (mColor == Color::pureBlue)
		mColor = Color::pureRed;
}
void Hexa::softChangeColor()
{
	mStartTime = glfwGetTime() + abs(mScale);
	mScale = -abs(mScale);
	this->changeColor();
}
void Hexa::antiChangeColor()
{
	if (mColor == Color::pureWhite)
		mColor = Color::pureBlack;
	else if (mColor == Color::pureBlack)
		mColor = Color::pureWhite;
	else if (mColor == Color::pureRed)
		mColor = Color::pureBlue;
	else if (mColor == Color::pureYellow)
		mColor = Color::pureRed;
	else if (mColor == Color::pureBlue)
		mColor = Color::pureYellow;
}
void Hexa::softAntiChangeColor()
{
	mStartTime = glfwGetTime() + abs(mScale);
	mScale = -abs(mScale);
	this->antiChangeColor();
}

void Hexa::freeChangeColor(Color color)
{
	mStartTime = glfwGetTime() + abs(mScale);
	mScale = -abs(mScale);
	mColor = color;
}

void Hexa::setNear(int index, Hexa* hexa)
{
	mNear[index] = hexa;
}

void Hexa::mScaleAdd()
{
	if (!mDeleteMode)
	{
		if (mScale < 1)
			mScale = 1 * (glfwGetTime() - mStartTime);
		else if (mScale > 1)
			mScale = 1;
	}
	else
	{
		if (mScale < 0)
			mScale = 1 * (glfwGetTime() - mStartTime);
		else if (mScale > 0)
		{
			mScale = 0;
			deleted = true;
		}
	}
}

float Hexa::getScale()const
{
	return mScale;
}

void Hexa::deleteModeOn()
{
	mStartTime = glfwGetTime() + abs(mScale);
	mScale = -abs(mScale);
	mDeleteMode = true;
}
bool Hexa::getDeleteMode()const
{
	return mDeleteMode;
}
bool Hexa::isDeleted()const
{
	return mDeleteMode && mScale == 0;
}

int Hexa::distanceToCenter()const
{
	return std::max(std::max(abs(xPos), abs(yPos)), abs(zPos));
}

HexaButton::HexaButton(float xOffset, float yOffset, float side) : mXOffset(xOffset), mYOffset(yOffset), mSide(side), Hexa(0, 0, 0) {}
HexaButton::HexaButton(int xOffset, int yOffset, float side) : mSide(side), Hexa(0, 0, 0)
{
	mXOffset = (xOffset - aplct->getWidth() / 2) / (aplct->getWidth() / 2);
	mYOffset = (yOffset - aplct->getLength() / 2) / (aplct->getLength() / 2);
}
HexaButton::HexaButton(float side) : mXOffset(0), mYOffset(0), mSide(side), Hexa(0, 0, 0) {}

void HexaButton::setSide(float side)
{
	mSide = side;
}

float HexaButton::getCenterXPos()const
{
	return Hexa::getCenterXPos(mSide) + mXOffset;
}
float HexaButton::getCenterXPos(float side, float rotation)const
{
	return Hexa::getCenterXPos(mSide) + mXOffset;
}
float HexaButton::getCenterYPos()const
{
	return Hexa::getCenterYPos(mSide) + mYOffset;
}
float HexaButton::getCenterYPos(float side, float rotation)const
{
	return Hexa::getCenterYPos(mSide) + mYOffset;
}
float HexaButton::getVertexXPos(int number, float scale = 1)const
{
	return Hexa::getVertexXPos(mSide, number, scale) + mXOffset;
}
float HexaButton::getVertexXPos(float side, int number, float scale, float rotation)const
{
	return Hexa::getVertexXPos(mSide, number, scale) + mXOffset;
}
float HexaButton::getVertexYPos(int number, float scale = 1)const
{
	return Hexa::getVertexYPos(mSide, number, scale) + mYOffset;
}
float HexaButton::getVertexYPos(float side, int number, float scale, float rotation)const
{
	return Hexa::getVertexYPos(mSide, number, scale) + mYOffset;
}

bool HexaButton::ifPositionInHexa(float x, float y, float scale = 1, float rotation = 0)const
{
	return Hexa::ifPositionInHexa(x, y, mSide, scale, rotation);
}
bool HexaButton::ifPositionInHexa(int x, int y, float scale = 1, float rotation = 0) const
{
	float fx = ((float)x - (float)(aplct->getWidth() / 2)) / (float)(aplct->getWidth() / 2);
	float fy = ((float)y - (float)(aplct->getLength() / 2)) / (float)(aplct->getLength() / 2);
	return ifPositionInHexa(fx, fy, scale, rotation);
}

void HexaButton::addText(const std::wstring& text, float xOffset, float yOffset, float scale, const float r, const float g, const float b, const float a)
{
	mTextList.push_back(TextTask({ text, xOffset, yOffset, scale, r, g, b, a }));
}

void HexaButton::deleteText(int index)
{
	mTextList.erase(mTextList.begin() + index);
}

void HexaButton::loadAllText()
{
	for (auto& text : mTextList)
	{
		float sinScale = abs(sin(this->getScale() * PI / 2));
		float x = (this->getCenterXPos() + sinScale * text.x);
		float y = (this->getCenterYPos() * ScreenRatio + sinScale * text.y);
		txtdp->loadText(text.text, x, y, sinScale * text.scale, text.r, text.g, text.b, sinScale * text.a);
	}
}