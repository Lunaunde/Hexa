#include "render.h"
#include <cmath>
#include"../glframework/core.h"
#include"../glframework/shader.h"
#include"../wrapper/GLErrorCheck.h"
#include"../application/Application.h"

const double PI = 3.14159265358979323846;
const double ScreenRatio = 16.0 / 9.0;

GLuint Render::hexaEBO = 0;
GLuint Render::hexaEBOL = 0;

Render::Render() {}
Render::~Render() {}

void Render::init()
{
	init_hexaEBO();
	init_hexaEBOL();
}

void Render::hexaRender(float xPos, float yPos, float r, float color[3])
{
	float pos[21], colors[21];
	Shader shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
	shader.begin();

	pos[0] = xPos;  // 中心点
	pos[1] = yPos * ScreenRatio;
	pos[2] = 0;
	colors[0] = color[0];
	colors[1] = color[1];
	colors[2] = color[2];

	for (int i = 1; i < 7; i++) { // 从第1个顶点开始计算
		double angle = 2 * PI * (i - 1) / 6;
		pos[i * 3] = xPos + r * sin(angle);
		pos[i * 3 + 1] = (yPos + r * cos(angle)) * ScreenRatio;
		pos[i * 3 + 2] = 0;
		colors[i * 3] = color[0];
		colors[i * 3 + 1] = color[1];
		colors[i * 3 + 2] = color[2];
	}

	GLuint vboPos = 0;
	GL_CALL(glGenBuffers(1, &vboPos));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboPos));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW));
	GLuint vboCol = 0;
	GL_CALL(glGenBuffers(1, &vboCol));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboCol));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW));

	GLuint vao = 0;
	GL_CALL(glGenVertexArrays(1, &vao));
	GL_CALL(glBindVertexArray(vao));

	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboPos));
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboCol));
	GL_CALL(glEnableVertexAttribArray(1));
	GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));

	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Render::hexaEBO));
	shader.setInt("type", 0);
	GL_CALL(glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, (void*)0));

	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Render::hexaEBOL));
	shader.setInt("type", 2);
	shader.setVec3("uColor", 0.5f, 0.5f, 0.5f);
	GL_CALL(glLineWidth(8.0f));
	GL_CALL(glDrawElements(GL_LINES,12,GL_UNSIGNED_INT,(void*)0))

	GL_CALL(glBindVertexArray(0));

	GL_CALL(glDeleteBuffers(1, &vboPos));
	GL_CALL(glDeleteBuffers(1, &vboCol));
	GL_CALL(glDeleteVertexArrays(1, &vao));
}

void Render::hexasRender(const std::vector<Hexa>& hexas, float r)
{
	for (auto& hexa : hexas)
	{
		float color[3];
		switch (hexa.getColor())
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
		hexaRender((hexa.getXPos()*r*sqrt(3))+(hexa.getYPos()*r*0.5*sqrt(3)), hexa.getYPos() * r * 1.5, r, color);
	}
}

void Render::init_hexaEBO()
{
	unsigned int indices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   0, 3, 4,
	   0, 4, 5,
	   0, 5, 6,
	   0, 6, 1
	};
	GL_CALL(glGenBuffers(1, &Render::hexaEBO));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Render::hexaEBO));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));
}

void Render::init_hexaEBOL()
{
	unsigned int indices[] = {
	   1, 2,
	   2, 3,
	   3, 4,
	   4, 5,
	   5, 6,
	   6, 1
	};
	GL_CALL(glGenBuffers(1, &Render::hexaEBOL));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Render::hexaEBOL));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));
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