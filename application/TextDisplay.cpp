#include"TextDisplay.h"
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

bool TextDisplay::init(std::string fontPath)
{
	if (FT_Init_FreeType(&mLibrary))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return false;
	}
	if (FT_New_Face(mLibrary, fontPath.c_str(), 0, &mFace))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return false;
	}
	FT_Set_Pixel_Sizes(mFace, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用byte-alignment限制
	for (GLubyte c = 0; c < 128; c++)
	{
		// 加载字符的字形 
		if (FT_Load_Char(mFace, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// 生成字形纹理
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
		//设置纹理选项
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// 将字符存储到字符表中备用Size
		Character character = {
			texture,
			mFace->glyph->bitmap.width,
			mFace->glyph->bitmap.rows,
			mFace->glyph->bitmap_left,
			mFace->glyph->bitmap_top,
			mFace->glyph->advance.x
		};
		Characters.insert(std::pair<wchar_t, Character>(c, character));
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

void TextDisplay::renderText(std::wstring text, GLfloat x, GLfloat y, GLfloat scale, GLfloat colorR, GLfloat colorG, GLfloat colorB)
{
	loadChineseCharacters(text);
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


	// 对文本中的所有字符迭代Character yBearing
	std::wstring::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.xBearing * scale;
		GLfloat ypos = y - (ch.ySize - ch.yBearing) * scale;

		GLfloat w = ch.xSize * scale;
		GLfloat h = ch.ySize * scale;
		// 当前字符的VBO
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// 在方块上绘制字形纹理
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// 更新当前字符的VBO
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// 绘制方块
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// 更新位置到下一个字形的原点，注意单位是1/64像素
		x += (ch.Advance >> 6) * scale; //(2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	mShader.setInt("text", 0);
	mShader.end();
}

void TextDisplay::loadChineseCharacters(std::wstring text)
{
	auto it = Characters.begin();
	while (it != Characters.end()) {
		if (it->first > 127) { // ASCII范围0-127
			glDeleteTextures(1, &it->second.textureID);
			it = Characters.erase(it);
		}
		else {
			++it;
		}
	}
	for (int i = 0; i < text.length(); i++)
	{
		if (Characters.count(text[i]) > 0) continue;
		if (text[i] >= 0x4e00 && text[i] <= 0x9fa5)
		{
			// 加载字符的字形 
			if (FT_Load_Char(mFace, text[i], FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// 生成字形纹理
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
			//设置纹理选项
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// 将字符存储到字符表中备用Size
			Character character = {
				texture,
				mFace->glyph->bitmap.width,
				mFace->glyph->bitmap.rows,
				mFace->glyph->bitmap_left,
				mFace->glyph->bitmap_top,
				mFace->glyph->advance.x
			};
			Characters.insert(std::pair<GLuint, Character>(text[i], character));
		}
	}
}