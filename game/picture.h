#ifndef PICTURE_H
#define PICTURE_H
#include<array>
#include"../glframework/core.h"
#include"../glframework/shader.h"
#include"../glframework/texture.h"

class Picture
{
public:
	Picture(std::string path, float x, float y, int weight, int height);
	Picture(std::string path, float x, float y, float scale);
	~Picture();

	void draw();

	void showON();
	void showOFF();

	void setAlpha(float alpha);
	void changeAlpha(float alpha, float time);
	float getAlpha();

	void zoomIn();
	void zoomOut();
	void zoomOff();
	void zoomCalculate();
	float getZoom();
	bool inPicture(int x, int y);
private:
	static Shader* shader;
	void init();
	Texture* mTexture;
	float x, y;
	int weight, height;

	bool isAlphaChanging = false;
	float targetAlpha = 1;
	float lastAlpha = 1;
	float alphaChangeTime;
	float alphaChageStartTime;

	float zoomClock;
	float zoom = 1;
	bool zoomState = false;
	bool isZoom = false;

	GLuint mVAO = 0, mVBOPos = 0, mVBOUV = 0, mEBO = 0;
	bool isShow = false;
	static constexpr std::array<float, 8> UV =
	{
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};
	static constexpr std::array<unsigned int, 6> indices = {
		0, 1, 2,
		1, 2, 3
	};
};

#endif