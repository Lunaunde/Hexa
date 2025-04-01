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
	void hexaDataLoad(Hexa hexa, float r, float color[3], float scale);
	void hexasDataLoad(const std::vector<Hexa>& hexas, float r);
	void test();
private:
	static Render* instance;
	Shader* mShader;
	std::vector<unsigned int> mPositionType;
	std::vector<float> mPosition;
	std::vector<unsigned int> mColorType;
	std::vector<float> mColor;
	std::vector<unsigned int> mIndices;
	GLuint ebo;
	GLuint vao;
	GLuint vboPositionType;
	GLuint vboPos;
	GLuint vboColorType;
	GLuint vboColor;
	Render();
};

#endif