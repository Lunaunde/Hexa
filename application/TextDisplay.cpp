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
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 初始化VBO/VAO
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    isInitialized = true;
    return true;
}

void TextDisplay::loadCharacter(wchar_t c)
{
    if (FT_Load_Char(mFace, c, FT_LOAD_RENDER))
    {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
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
    // 更新LRU顺序
    if (cacheMap.find(c) != cacheMap.end()) {
        lruList.erase(cacheMap[c]);
    }
    lruList.push_front(c);
    cacheMap[c] = lruList.begin();

    // 淘汰旧缓存
    if (cacheMap.size() > MAX_CACHE_SIZE) {
        wchar_t oldChar = lruList.back();
        glDeleteTextures(1, &Characters[oldChar].textureID);
        Characters.erase(oldChar);
        cacheMap.erase(oldChar);
        lruList.pop_back();
    }
}

void TextDisplay::renderText(std::wstring text, GLfloat x, GLfloat y, GLfloat scale, GLfloat colorR, GLfloat colorG, GLfloat colorB)
{
    if (!isInitialized) return;

    // 预加载文本中需要的所有字符
    for (wchar_t c : text) {
        if (c < 0x4e00 || c > 0x9fa5) continue; // 跳过非中文字符
        if (Characters.find(c) == Characters.end()) {
            loadCharacter(c);
        }
    }

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

    // 渲染文本
    GLfloat currentX = x;
    for (wchar_t c : text) {
        if (Characters.find(c) == Characters.end()) continue;

        Character ch = Characters[c];
        GLfloat xpos = currentX + ch.xBearing * scale;
        GLfloat ypos = y - (ch.ySize - ch.yBearing) * scale;
        GLfloat w = ch.xSize * scale;
        GLfloat h = ch.ySize * scale;

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

        // 更新位置
        currentX += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    mShader.end();
}