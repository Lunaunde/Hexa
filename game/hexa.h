#ifndef HEXA_H
#define HEXA_H

class Hexa
{
public:
	Hexa(int x, int y, int z);
	~Hexa();

	int getXPos()const;
	int getYPos()const;
	int getZPos()const;
	char getColor()const;

	Hexa* getNear(int index)const;

	float getCenterXPos(float side)const;
	float getCenterYPos(float side)const;
	float getCenterXPos(float side, float rotation)const;
	float getCenterYPos(float side, float rotation)const;
	float getVertexXPos(float side, int number, float scale)const;
	float getVertexXPos(float side, int number)const;
	float getVertexXPos(float side, int number, float scale, float rotation)const;
	float getVertexYPos(float side, int number, float scale)const;
	float getVertexYPos(float side, int number)const;
	float getVertexYPos(float side, int number, float scale, float rotation)const;

	bool ifPositionInHexa(float x, float y, float side, float scale, float rotation)const;
	bool ifPositionInHexa(int x, int y, float side)const;

	void setColor(char color);
	void setNear(int index, Hexa* hexa);

	int distanceToCenter()const;
private:
	char mColor;
	const int xPos;
	const int yPos;
	const int zPos;
	Hexa* mNear[6] = { nullptr,nullptr,nullptr,nullptr,nullptr,nullptr };
};

#endif
