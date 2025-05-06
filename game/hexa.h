#ifndef HEXA_H
#define HEXA_H

#include<vector>
#include"color.h"
#include"../application/TextDisplay.h"

class Hexa
{
public:
	Hexa(int x, int y, int z);
	~Hexa();

	int getXPos()const;
	int getYPos()const;
	int getZPos()const;

	Color getColor()const;
	Color getShowColor()const;

	Hexa* getNear(int index)const;

	virtual float getCenterXPos(float side)const;
	virtual float getCenterYPos(float side)const;
	virtual float getCenterXPos(float side, float rotation)const;
	virtual float getCenterYPos(float side, float rotation)const;
	virtual float getVertexXPos(float side, int number, float scale)const;
	virtual float getVertexXPos(float side, int number, float scale, float rotation)const;
	virtual float getVertexYPos(float side, int number, float scale)const;
	virtual float getVertexYPos(float side, int number, float scale, float rotation)const;

	virtual bool ifPositionInHexa(float x, float y, float side, float scale, float rotation)const;
	bool ifPositionInHexa(int x, int y, float side)const;
	bool ifPositionInHexa(int x, int y, float side, float scale, float rotation)const;

	void setColor(Color color);

	void changeColor();
	void softChangeColor();
	void antiChangeColor();
	void softAntiChangeColor();

	void freeChangeColor(Color color);
	void setNear(int index, Hexa* hexa);

	void mScaleAdd();

	float getScale()const;

	void deleteModeOn();
	bool getDeleteMode()const;
	bool isDeleted()const;

	int distanceToCenter()const;
private:
	Color mColor;
	mutable Color mShowColor;
	const int xPos;
	const int yPos;
	const int zPos;
	Hexa* mNear[6] = { nullptr,nullptr,nullptr,nullptr,nullptr,nullptr };

	float mScale;
	float mStartTime;

	bool mDeleteMode = false;
	bool deleted = false;
};

class HexaButton : public Hexa
{
public:
	HexaButton(float xOffset, float yOffset, float side);
	HexaButton(int xOffset, int yOffset, float side);
	HexaButton(float side);

	void setSide(float side);

	virtual float getCenterXPos() const;
	virtual float getCenterXPos(float side, float rotation) const;
	virtual float getCenterYPos() const;
	virtual float getCenterYPos(float side, float rotation) const;
	virtual float getVertexXPos(int number, float scale) const;
	virtual float getVertexXPos(float side, int number, float scale, float rotation) const;
	virtual float getVertexYPos(int number, float scale) const;
	virtual float getVertexYPos(float side, int number, float scale, float rotation) const;
	virtual bool ifPositionInHexa(float x, float y, float scale, float rotation) const;
	virtual bool ifPositionInHexa(int x, int y, float scale, float rotation) const;

	void addText(const std::wstring& text, float xOffset, float yOffset, float scale, const float r, const float g, const float b, const float a);
	void deleteText(int index);
	void loadAllText();

private:
	float mXOffset = 0;
	float mYOffset = 0;
	float mSide = 0;

	std::vector<TextTask> mTextList;
};

#endif
