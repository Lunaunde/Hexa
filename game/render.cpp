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
	GL_CALL(glGenBuffers(1, &vboPos));
	GL_CALL(glGenBuffers(1, &vboColor));
	GL_CALL(glGenVertexArrays(1, &vao));
	GL_CALL(glGenBuffers(1, &ebo));
	GL_CALL(glLineWidth(8.0f));
}

void Render::dataLoad()
{
	mTrianglesPosition.clear();
	mTrianglesColor.clear();
	mLinesPosition.clear();
	mLinesColor.clear();
	mTrianglesIndices.clear();
	mLinesIndices.clear();
	hexasDataLoad(sta->getHexas(), sta->getHexaRadius());
}

void Render::draw()
{
	mShader->begin();

	GL_CALL(glBindVertexArray(vao));

	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboPos));
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboColor));
	GL_CALL(glEnableVertexAttribArray(1));
	GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));

	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboPos));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, mTrianglesPosition.size() * sizeof(float), mTrianglesPosition.data(), GL_STATIC_DRAW));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboColor));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, mTrianglesColor.size() * sizeof(float), mTrianglesColor.data(), GL_STATIC_DRAW));

	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mTrianglesIndices.size() * sizeof(unsigned int), mTrianglesIndices.data(), GL_STATIC_DRAW));
	mShader->setInt("type", 0);
	GL_CALL(glDrawElements(GL_TRIANGLES, mTrianglesIndices.size(), GL_UNSIGNED_INT, (void*)0));

	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboPos));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, mLinesPosition.size() * sizeof(float), mLinesPosition.data(), GL_STATIC_DRAW));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboColor));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, mLinesColor.size() * sizeof(float), mLinesColor.data(), GL_STATIC_DRAW));

	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mLinesIndices.size() * sizeof(unsigned int), mLinesIndices.data(), GL_STATIC_DRAW));
	mShader->setInt("type", 2);
	mShader->setVec3("uColor", 0.5f, 0.5f, 0.5f);
	GL_CALL(glDrawElements(GL_LINES, mLinesIndices.size(), GL_UNSIGNED_INT, (void*)0));

	GL_CALL(glBindVertexArray(0));
	mShader->end();
}

void Render::hexaDataLoad(Hexa hexa, float r, float color[3], int num)
{
	std::array<float, 21> pos, colors;
	std::array<unsigned int, 18> indices = {
	   0, 1, 2,
	   0, 2, 3,
	   0, 3, 4,
	   0, 4, 5,
	   0, 5, 6,
	   0, 6, 1
	};
	std::array<unsigned int, 18> indicesL = {
		1, 2,
		2, 3,
		3, 4,
		4, 5,
		5, 6,
		6, 1
	};

	pos[0] = hexa.getRenderXPos(r);  // 中心点
	pos[1] = hexa.getRenderYPos(r) * ScreenRatio;
	pos[2] = 0;
	colors[0] = color[0];
	colors[1] = color[1];
	colors[2] = color[2];

	for (int i = 1; i < 7; i++) { // 从第1个顶点开始计算
		pos[i * 3] = hexa.getVertexXPos(r, i);
		pos[i * 3 + 1] = hexa.getVertexYPos(r, i) * ScreenRatio;
		pos[i * 3 + 2] = 0;
		colors[i * 3] = color[0];
		colors[i * 3 + 1] = color[1];
		colors[i * 3 + 2] = color[2];
	}
	for (int i = 0;i < indices.size();i++)
	{
		indices[i] += 7 * num;
	}
	for (int i = 0;i < indicesL.size();i++)
	{
		indicesL[i] += 7 * num;
	}

	mTrianglesPosition.insert(mTrianglesPosition.end(), std::begin(pos), std::end(pos));
	mTrianglesColor.insert(mTrianglesColor.end(), std::begin(colors), std::end(colors));
	mLinesPosition.insert(mLinesPosition.end(), std::begin(pos), std::end(pos));
	mLinesColor.insert(mLinesColor.end(), std::begin(colors), std::end(colors));
	mTrianglesIndices.insert(mTrianglesIndices.end(), std::begin(indices), std::end(indices));
	mLinesIndices.insert(mLinesIndices.end(), std::begin(indicesL), std::end(indicesL));
}

void Render::hexasDataLoad(const std::vector<Hexa>& hexas, float r)
{
	for (int i = 0; i < hexas.size(); i++)
	{
		float color[3];
		switch (hexas[i].getColor())
		{
		case'W':
		{
			color[0] = 1.0f;	color[1] = 1.0f;	color[2] = 1.0f;
		}
		break;
		case 'B':
		{
			color[0] = 0.0f;	color[1] = 0.0f;	color[2] = 0.0f;
		}
		break;
		default:
		{
			color[0] = 0.5f;	color[1] = 0.5f;	color[2] = 0.5f;
		}
		break;
		}
		hexaDataLoad(hexas[i], r, color, i);
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