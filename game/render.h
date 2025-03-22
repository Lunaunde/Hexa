#ifndef RENDER_H
#define RENDER_H
#include<vector>
#include"../glframework/core.h"
#include"../glframework/shader.h"
#include"state.h"

class Render
{
public:
	Render();
	~Render();
	static void init();
	static void hexaRender(Hexa hexa, float r, float color[3],int num);
	static void hexasRender(const std::vector<Hexa>& hexas, float r);
	static void test();
	static void destroy();
private:
	static void init_hexaEBO();
	static void init_hexaEBOL();
	static Shader* shader;
	static std::vector<float> aPosition;
	static std::vector<float> aColor;
	static std::vector<unsigned int> aIndices;
	static std::vector<unsigned int> aIndicesL;
	static GLuint ebo;
	static GLuint eboL;
	static GLuint vao;
    static GLuint vboPos;
    static GLuint vboColor;
};

#endif