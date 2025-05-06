#include <cmath>
#include <array>
#include "render.h"
#include"../glframework/core.h"
#include"../wrapper/GLErrorCheck.h"
#include"../application/Application.h"

const double PI = 3.14159265358979323846;
const double ScreenRatio = 16.0 / 9.0;

Render* Render::instance = nullptr;

Render::~Render()
{
	mShader->end();
	delete mShader;
	GL_CALL(glDeleteBuffers(1, &vboPositionType));
	GL_CALL(glDeleteBuffers(1, &vboPos));
	GL_CALL(glDeleteBuffers(1, &vboPos));
	GL_CALL(glDeleteBuffers(1, &vboColor));
	GL_CALL(glDeleteVertexArrays(1, &vao));
	instance = nullptr;
}

Render* Render::getInstance()
{
	if (instance == nullptr)
		instance = new Render();
	return instance;
}

void Render::init()
{
	mShader = new Shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
	GL_CALL(glGenBuffers(1, &vboPositionType))
		GL_CALL(glGenBuffers(1, &vboPos));
	GL_CALL(glGenBuffers(1, &vboColorType));
	GL_CALL(glGenBuffers(1, &vboColor));
	GL_CALL(glGenVertexArrays(1, &vao));
	GL_CALL(glGenBuffers(1, &ebo));
	GL_CALL(glLineWidth(8.0f));
}

void Render::clear()
{
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void Render::dataLoad()
{
	mPosition.clear();
	mColor.clear();
	mIndices.clear();
	mColorType.clear();
	mPositionType.clear();
	std::vector<Hexa*> hexas;
	for (auto& hexa : sta->getHexaButtons())
		hexas.push_back(&hexa);
	for (auto& hexa : sta->getHexas())
		hexas.push_back(&hexa);
	hexasDataLoad(hexas, sta->getHexaRadius());
}

void Render::draw()
{
	mShader->begin();

	GL_CALL(glBindVertexArray(vao));

	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboPositionType));
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(unsigned int), (void*)0));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, mPositionType.size() * sizeof(unsigned int), mPositionType.data(), GL_STATIC_DRAW));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboPos));
	GL_CALL(glEnableVertexAttribArray(1));
	GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, mPosition.size() * sizeof(float), mPosition.data(), GL_STATIC_DRAW));

	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboColorType));
	GL_CALL(glEnableVertexAttribArray(8));
	GL_CALL(glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(unsigned int), (void*)0));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, mColorType.size() * sizeof(unsigned int), mColorType.data(), GL_STATIC_DRAW));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboColor));
	GL_CALL(glEnableVertexAttribArray(9));
	GL_CALL(glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, mColor.size() * sizeof(float), mColor.data(), GL_STATIC_DRAW));

	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.data(), GL_STATIC_DRAW));
	GL_CALL(glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, (void*)0));

	mShader->end();
}

void Render::hexaDataLoad(Hexa* hexa, float side, float color[3], float scale)
{
	int startNumber = mPosition.size() / 3;
	std::array<float, 21> pos, colors;
	std::array<unsigned int, 18> indices = {
	   0, 1, 2,
	   0, 2, 3,
	   0, 3, 4,
	   0, 4, 5,
	   0, 5, 6,
	   0, 6, 1
	};
	std::array<unsigned int, 7> positionType = { 0,0,0,0,0,0,0 };
	std::array<unsigned int, 7> colorType = { 3,3,3,3,3,3,3 };

	float rotation = 0;
	if (sta->getRotationMode())
		rotation = glfwGetTime();

	pos[0] = hexa->getCenterXPos(side, rotation);  // 中心点
	pos[1] = hexa->getCenterYPos(side, rotation) * ScreenRatio;
	pos[2] = 0;
	colors[0] = color[0];
	colors[1] = color[1];
	colors[2] = color[2];

	for (int i = 1; i < 7; i++) { // 从第1个顶点开始计算
		pos[i * 3] = hexa->getVertexXPos(side, i, scale, rotation);
		pos[i * 3 + 1] = hexa->getVertexYPos(side, i, scale, rotation) * ScreenRatio;
		pos[i * 3 + 2] = 0;
		colors[i * 3] = color[0];
		colors[i * 3 + 1] = color[1];
		colors[i * 3 + 2] = color[2];
	}

	for (int i = 0; i < indices.size(); i++)
	{
		indices[i] += startNumber;
	}

	mPosition.insert(mPosition.end(), std::begin(pos), std::end(pos));
	mColor.insert(mColor.end(), std::begin(colors), std::end(colors));
	mIndices.insert(mIndices.end(), std::begin(indices), std::end(indices));
	mPositionType.insert(mPositionType.end(), std::begin(positionType), std::end(positionType));
	mColorType.insert(mColorType.end(), std::begin(colorType), std::end(colorType));
}

void Render::hexasDataLoad(const std::vector<Hexa*>& hexas, float side)
{
	float grey[3] = { 0.5f,0.5f,0.5f };
	for (int i = 0; i < hexas.size(); i++)
	{
		Color color = hexas[i]->getShowColor();
		float colors[3] = { color.getRed(),color.getGreen(), color.getBlue() };
		//hexaDataLoad(hexas[i], side, grey, 1.05);
		hexaDataLoad(hexas[i], side, colors, 0.90);
	}
}

void Render::test()
{
	Shader shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
	float positions[] = {
		-0.2f, -0.2f, 0.0f,
		0.2f, -0.2f, 0.0f,
		-0.2f,  0.2f, 0.0f,
		0.2f,  0.2f, 0.0f,
	};

	float colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f,  0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
	};

	float uvs[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 1, 3
	};

	GLuint vboPos = 0;
	glGenBuffers(1, &vboPos);
	glBindBuffer(GL_ARRAY_BUFFER, vboPos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	GLuint vboCol = 0;
	glGenBuffers(1, &vboCol);
	glBindBuffer(GL_ARRAY_BUFFER, vboCol);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	GLuint vboUV = 0;
	glGenBuffers(1, &vboUV);
	glBindBuffer(GL_ARRAY_BUFFER, vboUV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);

	GLuint ebo = 0;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vboPos);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);	//vbo通过glvVertexAttibPointer和vao进行绑定
	glBindBuffer(GL_ARRAY_BUFFER, vboCol);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);	//vbo通过glvVertexAttibPointer和vao进行绑定
	glBindBuffer(GL_ARRAY_BUFFER, vboUV);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);	//ebo通过glBindBuffer和vao绑定

	shader.begin();
	shader.setInt("type", 0);

	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

	shader.end();

	glBindVertexArray(0);
}

Render::Render() : mShader(nullptr)
{
}

DistortedBackground::DistortedBackground()
{
	mShader = new Shader("assets/shaders/vertex_DB.glsl", "assets/shaders/fragment_DB.glsl");
}
DistortedBackground::~DistortedBackground()
{
	delete mShader;
}
DistortedBackground* DistortedBackground::instance = nullptr;
DistortedBackground* DistortedBackground::getInstance()
{
	if (instance == nullptr)
		instance = new DistortedBackground();
	return instance;
}
void DistortedBackground::init()
{
	vao = 0;
	GL_CALL(glGenVertexArrays(1, &vao));
	GL_CALL(glBindVertexArray(vao));
	vbo = 0;
	GL_CALL(glGenBuffers(1, &vbo));
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GL_CALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), vertices.data(), GL_STATIC_DRAW));
	ebo = 0;
	GL_CALL(glGenBuffers(1, &ebo));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));
	GL_CALL(glBindVertexArray(0));
}

void DistortedBackground::draw()
{
	mShader->begin();
	mShader->setVec2("iResolution", aplct->getWidth(), aplct->getLength());
	mShader->setFloat("iTime", glfwGetTime());
	GL_CALL(glBindVertexArray(vao));
	GL_CALL(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0));
	GL_CALL(glBindVertexArray(0));
	mShader->end();
}


CrystalBackground::CrystalBackground()
{
	mShader = new Shader("assets/shaders/vertex_C.glsl", "assets/shaders/fragment_C.glsl");
}
CrystalBackground::~CrystalBackground()
{
	delete mShader;
}
CrystalBackground* CrystalBackground::instance = nullptr;
CrystalBackground* CrystalBackground::getInstance()
{
	if (instance == nullptr)
		instance = new CrystalBackground();
	return instance;
}
Texture* test;
void CrystalBackground::init()
{
	mTexture = new Texture("assets/textures/background01.png", 0);
	test = new Texture("assets/textures/p.jpeg", 0);
	vao = 0;
	GL_CALL(glGenVertexArrays(1, &vao));
	GL_CALL(glBindVertexArray(vao));

	vbo = 0;
	GL_CALL(glGenBuffers(1, &vbo));
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW));

	GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0)); // 位置
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)))); // 纹理坐标
	GL_CALL(glEnableVertexAttribArray(1));

	ebo = 0;
	GL_CALL(glGenBuffers(1, &ebo));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));
	GL_CALL(glBindVertexArray(0));
}

void CrystalBackground::draw()
{
	mShader->begin();
	mShader->setInt("uBackground", 0);
	mTexture->bind();
	mShader->setVec2("uResolution", aplct->getWidth(), aplct->getLength());
	mShader->setFloat("uTime", glfwGetTime());
	mShader->setFloat("uEta", 0.75f);
	mShader->setFloat("uDistortionStrength", 0.1f);
	mShader->setFloat("uNoiseScale", 8.0f);

	GL_CALL(glBindVertexArray(vao));
	GL_CALL(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0));
	GL_CALL(glBindVertexArray(0));
	mShader->end();
}