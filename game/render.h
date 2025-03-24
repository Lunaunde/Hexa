#ifndef RENDER_H
#define RENDER_H
#define rdr Render::getInstance()
#include<vector>
#include"../glframework/core.h"
#include"../glframework/shader.h"
#include"state.h"

class Render
{
public:
	~Render();
    static Render* getInstance();
	void init();
	void dataLoad();
	void draw();
	void hexaDataLoad(Hexa hexa, float r, float color[3],int num);
	void hexasDataLoad(const std::vector<Hexa>& hexas, float r);
	void test();
private:
	static Render * instance;
	Shader* mShader;
	std::vector<float> mTrianglesPosition;
	std::vector<float> mTrianglesColor;
	std::vector<float> mLinesPosition;
	std::vector<float> mLinesColor;
	std::vector<unsigned int> mTrianglesIndices;
	std::vector<unsigned int> mLinesIndices;
	GLuint ebo;
	GLuint vao;
    GLuint vboPos;
    GLuint vboColor;
	Render();
};

#endif