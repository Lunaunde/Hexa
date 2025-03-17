#ifndef RENDER_H
#define RENDER_H
#include<vector>
#include"../glframework/core.h"
#include"state.h"

class Render
{
public:
    Render();
    ~Render();
    static void init();
    static void hexaRender(float xPos,float yPos, float r,float color[3]);
    static void hexasRender(const std::vector<Hexa>& hexas,float r);
    static GLuint hexaEBO;
    static GLuint hexaEBOL;
    static void test();
private:
    static void init_hexaEBO();
    static void init_hexaEBOL();
};

#endif