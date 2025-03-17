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
    float getRenderXPos(float r)const;
    float getRenderYPos(float r)const;
    float getVertexXPos(float r,int number)const;
    float getVertexYPos(float r,int number)const;
    Hexa* getNear(int index)const;

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
