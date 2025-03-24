#ifndef TEXT_DISPLAY_H
#define TEXT_DISPLAY_H
#define txtdp TextDisplay::getInstance()
#include <ft2build.h>
#include<string>
#include<map>
#include"../glframework/core.h"
#include"../glframework/shader.h"
#include FT_FREETYPE_H

struct Character {
	GLuint     textureID;  // 字形纹理ID
	int xSize, ySize;       // 字形大大小
	int xBearing, yBearing;    // 字形基于基线和起点的位置
	GLuint     Advance;    // 起点到下一个字形起点的距离
};

class TextDisplay
{
public:
	~TextDisplay();
	static TextDisplay* getInstance();
	bool init(std::string fontPath);
	void renderText(std::wstring text, GLfloat x, GLfloat y, GLfloat scale, GLfloat colorR, GLfloat colorG, GLfloat colorB);

private:
	void loadChineseCharacters(std::wstring text);
	static TextDisplay* instance;
	Shader mShader;
	FT_Library mLibrary;
	FT_Face mFace;
	GLuint mVAO, mVBO;
	std::map<wchar_t, Character> Characters;
	TextDisplay();
};

#endif