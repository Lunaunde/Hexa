#ifndef RENDER_H
#define RENDER_H
#define rdr Render::getInstance()
#define dtbg DistortedBackground::getInstance()
#include<vector>
#include"../glframework/core.h"
#include"../glframework/shader.h"
#include"../glframework/texture.h"
#include"state.h"

class Render
{
public:
	~Render();
	static Render* getInstance();
	void init();
	void dataLoad();
	void draw();
	void hexaDataLoad(Hexa hexa, float side, float color[3], float scale);
	void hexasDataLoad(const std::vector<Hexa>& hexas, float side);
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

class DistortedBackground
{
public:
	~DistortedBackground();
	static DistortedBackground* getInstance();
	void init();
	void draw();
private:
	static DistortedBackground* instance;
	Shader* mShader;
	static constexpr std::array<float, 16> vertices = {
		-1.0f,  1.0f, 0.0f, 1.0f, 
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f
	};
	static constexpr std::array<unsigned int, 6> indices = {
		0, 1, 2,
		0, 2, 3
	};
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	DistortedBackground();
};

class CrystalBackground
{
public:
	~CrystalBackground();
	static CrystalBackground* getInstance();
	void init();
	void draw();
private:
	static CrystalBackground* instance;
	Shader* mShader;
	Texture* mTexture;
	static constexpr std::array<float, 16> vertices = {
		-1.0f,  1.0f, 0.1f, 0.9f,
		-1.0f, -1.0f, 0.1f, 0.1f,
		 1.0f, -1.0f, 0.9f, 0.1f,
		 1.0f,  1.0f, 0.9f, 0.9f
	};
	static constexpr std::array<unsigned int, 6> indices = {
		0, 1, 2,
		0, 2, 3
	};
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	CrystalBackground();
};
#endif