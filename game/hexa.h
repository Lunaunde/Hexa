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

    float getCenterXPos(float r)const;
    float getCenterYPos(float r)const;
    float getVertexXPos(float r, int number, float scale)const;
    float getVertexXPos(float r,int number)const;
    float getVertexYPos(float r, int number, float scale)const;
    float getVertexYPos(float r,int number)const;

    bool ifPositionInHexa(float x, float y, float r)const;
    bool ifPositionInHexa(int x, int y,float r)const;

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
