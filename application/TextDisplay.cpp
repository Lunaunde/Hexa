#include "TextDisplay.h"
#include"Application.h"
#include<iostream>

TextDisplay* TextDisplay::instance = nullptr;

TextDisplay::TextDisplay()
{
	mShader = new Shader("assets/shaders/vertex_text.glsl", "assets/shaders/fragment_text.glsl");
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

bool TextDisplay::init(const std::string& fontPath)
{
	if (FT_Init_FreeType(&mLibrary)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return false;
	}
	if (FT_New_Face(mLibrary, fontPath.c_str(), 0, &mFace)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return false;
	}
	FT_Set_Pixel_Sizes(mFace, 0, 128);
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

void TextDisplay::loadCharacter(wchar_t c)
{
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

void TextDisplay::updateCache(wchar_t c)
{
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

void TextDisplay::preloadASCII()
{
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

void TextDisplay::loadText(const std::wstring& text, float centerX, float centerY, float scale, const float r, const float g, const float b, const float a)
{
	if (text.empty()) return;

	// 计算文本实际尺寸
	TextMetrics metrics = calculateTextMetrics(text, scale);

	centerX = (centerX + 1) * (aplct->getWidth() / 2.0f);
	centerY = (centerY + 1) * (aplct->getLength() / 2.0f);

	float startX = centerX - metrics.totalWidth / 2.0f;
	float startY = centerY + metrics.maxBearingY / 2.0f - metrics.maxHeight / 2.0f;

	renderQueue.push_back({ text, startX, startY, scale, r, g, b, a });

	// 只加载缺失的中文字符
	for (wchar_t c : text) {
		if ((c >= 0x4e00 && c <= 0x9fa5) || (c >= 128)) { // 中文或非ASCII字符
			if (Characters.find(c) == Characters.end()) {
				loadCharacter(c);
			}
		}
	}
}

void TextDisplay::draw()
{
	if (!isInitialized || renderQueue.empty()) return;

	const float ortho[16] = {
		1.0f / 960.0f, 0.0f,        0.0f, 0.0f,
		0.0f,        1.0f / 540.0f, 0.0f, 0.0f,
		0.0f,        0.0f,       -1.0f, 0.0f,
		-1.0f,      -1.0f,        0.0f, 1.0f
	};

	mShader->begin();
	mShader->setMat4("projection", ortho);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(mVAO);

	for (const auto& task : renderQueue) {
		float currentX = task.x;
		float currentY = task.y;

		mShader->setFloat4("textColor", task.r, task.g, task.b, task.a);

		for (wchar_t c : task.text) {
			if (Characters.find(c) == Characters.end()) continue;

			Character ch = Characters[c];
			float xpos = currentX + ch.bearingX * task.scale;
			float ypos = currentY - (ch.height - ch.bearingY) * task.scale;
			float w = ch.width * task.scale;
			float h = ch.height * task.scale;

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

			currentX += (ch.advance >> 6) * task.scale;
		}
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	mShader->end();
}

void TextDisplay::clearQueue()
{
	renderQueue.clear();
}

TextMetrics TextDisplay::calculateTextMetrics(const std::wstring& text, float scale) {
	TextMetrics metrics = { 0.0f, 0.0f, 0.0f };

	for (wchar_t c : text) {
		if (Characters.find(c) == Characters.end()) continue;

		const Character& ch = Characters[c];

		// 累计宽度（考虑字符间距）
		metrics.totalWidth += (ch.advance >> 6) * scale;

		// 计算最大高度和基线位置
		float charHeight = ch.height * scale;
		float bearingY = ch.bearingY * scale;

		if (charHeight > metrics.maxHeight) {
			metrics.maxHeight = charHeight;
		}
		if (bearingY > metrics.maxBearingY) {
			metrics.maxBearingY = bearingY;
		}
	}

	return metrics;
}