#include "TextDisplay.h"
#include<iostream>

TextDisplay* TextDisplay::instance = nullptr;

TextDisplay::TextDisplay() : mShader("assets/shaders/vertex_text.glsl", "assets/shaders/fragment_text.glsl")
{
}

TextDisplay::~TextDisplay()
{
	for (auto& pair : Characters) {
		glDeleteTextures(1, &pair.second.textureID);
	}
	FT_Done_Face(mFace);
	FT_Done_FreeType(mLibrary);
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
	instance = nullptr;
}

TextDisplay* TextDisplay::getInstance()
{
	if (instance == nullptr)
		instance = new TextDisplay();
	return instance;
}

bool TextDisplay::init(const std::string& fontPath) {
	if (FT_Init_FreeType(&mLibrary)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return false;
	}
	if (FT_New_Face(mLibrary, fontPath.c_str(), 0, &mFace)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return false;
	}
	FT_Set_Pixel_Sizes(mFace, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// 初始化 VAO/VBO
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// 预加载 ASCII 字符
	preloadASCII();

	isInitialized = true;
	return true;
}

void TextDisplay::loadCharacter(wchar_t c) {
	if (FT_Load_Char(mFace, c, FT_LOAD_RENDER)) {
		std::wcout << L"Failed to load glyph: " << c << std::endl;
		return;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		mFace->glyph->bitmap.width,
		mFace->glyph->bitmap.rows,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		mFace->glyph->bitmap.buffer
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Character ch = {
		texture,
		mFace->glyph->bitmap.width,
		mFace->glyph->bitmap.rows,
		mFace->glyph->bitmap_left,
		mFace->glyph->bitmap_top,
		mFace->glyph->advance.x
	};
	Characters[c] = ch;
	updateCache(c);
}

void TextDisplay::updateCache(wchar_t c) {
	if (cacheMap.find(c) != cacheMap.end()) {
		lruList.erase(cacheMap[c]);
	}
	lruList.push_front(c);
	cacheMap[c] = lruList.begin();

	if (cacheMap.size() > MAX_CACHE_SIZE) {
		wchar_t oldChar = lruList.back();
		glDeleteTextures(1, &Characters[oldChar].textureID);
		Characters.erase(oldChar);
		cacheMap.erase(oldChar);
		lruList.pop_back();
	}
}

void TextDisplay::preloadASCII() {
	for (wchar_t c = 0; c < 128; ++c) {
		if (FT_Load_Char(mFace, c, FT_LOAD_RENDER)) {
			continue;
		}

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			mFace->glyph->bitmap.width,
			mFace->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			mFace->glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character ch = {
			texture,
			mFace->glyph->bitmap.width,
			mFace->glyph->bitmap.rows,
			mFace->glyph->bitmap_left,
			mFace->glyph->bitmap_top,
			mFace->glyph->advance.x
		};
		Characters[c] = ch;
	}
}

void TextDisplay::renderText(const std::wstring& text, float x, float y, float scale, GLfloat colorR, GLfloat colorG, GLfloat colorB) {
	if (!isInitialized) return;

	// 只加载缺失的中文字符
	for (wchar_t c : text) {
		if (c >= 0x4e00 && c <= 0x9fa5) { // 中文范围
			if (Characters.find(c) == Characters.end()) {
				loadCharacter(c);
			}
		}
	}

	// 设置渲染参数（正交投影等）
	const float ortho[16] = {
		1.0f / 960.0f, 0.0f,        0.0f, 0.0f,
		0.0f,        1.0f / 540.0f, 0.0f, 0.0f,
		0.0f,        0.0f,       -1.0f, 0.0f,
		-1.0f,      -1.0f,        0.0f, 1.0f
	};

	mShader.begin();
	mShader.setMat4("projection", ortho);
	mShader.setFloat3("textColor", colorR, colorG, colorB);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(mVAO);

	float currentX = x;

	for (wchar_t c : text) {
		if (Characters.find(c) == Characters.end()) continue;

		Character ch = Characters[c];
		float xpos = currentX + ch.xBearing * scale;
		float ypos = y - (ch.ySize - ch.yBearing) * scale;
		float w = ch.xSize * scale;
		float h = ch.ySize * scale;

		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};

		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		currentX += (ch.Advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	mShader.end();
}