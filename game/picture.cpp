#include "picture.h"
#include"../application/Application.h"
#include"../wrapper/GLErrorCheck.h"

Shader* Picture::shader = nullptr;//new Shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

Picture::Picture(std::string path, float x, float y, int weight, int height) : x(x), y(y), weight(weight), height(height)
{
	mTexture = new Texture(path, 0);
	init();
}
Picture::Picture(std::string path, float x, float y, float scale = 1) :x(x), y(y)
{
	mTexture = new Texture(path, 0);
	weight = (float)mTexture->getWidth() * scale;
	height = (float)mTexture->getHeight() * scale;
	init();
}
Picture::~Picture()
{
}
void Picture::init()
{
	if (shader == nullptr)
		shader = new Shader("assets/shaders/vertex_texture.glsl", "assets/shaders/fragment_texture.glsl");
	GL_CALL(glGenBuffers(1, &mVBOPos));
	GL_CALL(glGenBuffers(1, &mVBOUV));
	GL_CALL(glGenVertexArrays(1, &mVAO));
	GL_CALL(glGenBuffers(1, &mEBO));


	GL_CALL(glBindVertexArray(mVAO));
	GL_CALL(glEnableVertexAttribArray(1));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, mVBOUV));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, UV.size() * sizeof(float), UV.data(), GL_STATIC_DRAW));
	GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0)); // ÎÆÀí×ø±ê

	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));
	GL_CALL(glBindVertexArray(0));
}
void Picture::draw()
{
	if (!isShow)
		return;

	float hfw = (float)weight / aplct->getWidth();
	float hfh = (float)height / aplct->getLength();
	if (isZoom)
	{
		zoomCalculate();
        hfw *= sin(zoom * 3.1415926 / 2.0);
        hfh *= sin(zoom * 3.1415926 / 2.0);
	}

	float Postions[12] = {
		x - hfw, y - hfh, 0,
		x - hfw, y + hfh, 0,
		x + hfw, y - hfh, 0,
		x + hfw, y + hfh, 0
	};

	
	GL_CALL(glBindVertexArray(mVAO));
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, mVBOPos));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), Postions, GL_STATIC_DRAW));
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0)); // Î»ÖÃ

	shader->begin();
	if(isAlphaChanging)
		shader->setFloat("alpha", getAlpha());
	else
		shader->setFloat("alpha", lastAlpha);

	mTexture->bind();
	GL_CALL(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0));
	GL_CALL(glBindVertexArray(0));
	shader->end();
}

void Picture::showOFF()
{
	isShow = false;
}
void Picture::showON()
{
	isShow = true;
}

void Picture::setAlpha(float alpha)
{
	isAlphaChanging = false;
	targetAlpha = alpha;
	lastAlpha = alpha;
}
void Picture::changeAlpha(float alpha, float time)
{
	lastAlpha = getAlpha();
	isAlphaChanging = true;
	targetAlpha = alpha;
	alphaChangeTime = time;
	alphaChageStartTime = glfwGetTime();
}
float Picture::getAlpha()
{
	if (glfwGetTime() - alphaChageStartTime < alphaChangeTime)
	{
		return lastAlpha + (targetAlpha - lastAlpha) * (glfwGetTime() - alphaChageStartTime) / alphaChangeTime;
	}
	else
	{
        lastAlpha = targetAlpha;
		isAlphaChanging = false;
		return targetAlpha;
	}
}

void Picture::zoomIn()
{
	zoom = 0;
    showON();
    zoomState = true;
    isZoom = true;
    zoomClock = glfwGetTime();
}
void Picture::zoomOut()
{
	zoom = 1;
    zoomState = false;
    isZoom = true;
    zoomClock = glfwGetTime();
}
void Picture::zoomOff()
{
    isZoom = false;
}
void Picture::zoomCalculate()
{
    if (!isZoom)
        return;
    if (zoomState)
    {
        if (zoom < 1)
            zoom = glfwGetTime() - zoomClock;
        else if (zoom > 1)
        {
            zoom = 1;
            isZoom = false;
        }
    }
    else
    {
        if (zoom > 0)
            zoom = 1 + zoomClock - glfwGetTime();
        else if (zoom < 0)
        {
            zoom = 0;
            isZoom = false;
            isShow = false;
        }
    }
}
float Picture::getZoom()
{
	zoomCalculate();
	return zoom;
}

bool Picture::inPicture(int ix, int iy)
{
	if(!isShow)
        return false;
	float hfw = (float)weight / aplct->getWidth();
	float hfh = (float)height / aplct->getLength();
	if (isZoom)
	{
		zoomCalculate();
		hfw *= sin(zoom * 3.1415926 / 2.0);
		hfh *= sin(zoom * 3.1415926 / 2.0);
	}

	if ((x - hfw + 1) * (aplct->getWidth() / 2)<= ix && ix <= (x + hfw + 1) * (aplct->getWidth() / 2)&& (y - hfh + 1) * (aplct->getLength() / 2) <= iy && iy <= (y + hfh + 1) * (aplct->getLength() / 2))
		return true;
	return false;
}