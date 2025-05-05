#ifndef TEXT_DISPLAY_H
#define TEXT_DISPLAY_H
#define txtdp TextDisplay::getInstance()
#include <ft2build.h>
#include<string>
#include<map>
#include<list>
#include <vector>
#include<unordered_map>
#include"../glframework/core.h"
#include"../glframework/shader.h"
#include FT_FREETYPE_H


class Shader; // 假设你有Shader类

struct Character {
    GLuint textureID;
    GLuint width, height;
    GLint bearingX, bearingY;
    GLuint advance;
};

// 文字渲染任务结构体
struct TextTask {
    std::wstring text;
    float x, y;
    float scale;
    float r, g, b, a;
};

class TextDisplay {
public:
    static TextDisplay* getInstance();
    bool init(const std::string& fontPath);

    void loadText(const std::wstring& text, float x, float y, float scale, const float r,const float g,const float b,const float a);
    void draw();
    void clearQueue();

private:
    static TextDisplay* instance;
    TextDisplay();
    ~TextDisplay();

    GLuint mVAO, mVBO;
    Shader* mShader;
    FT_Library mLibrary;
    FT_Face mFace;

    std::map<wchar_t, Character> Characters;
    std::list<wchar_t> lruList;
    std::map<wchar_t, std::list<wchar_t>::iterator> cacheMap;
    static const size_t MAX_CACHE_SIZE = 4096;

    void loadCharacter(wchar_t c);
    void updateCache(wchar_t c);
    void preloadASCII();

    std::vector<TextTask> renderQueue;

    bool isInitialized = false;
};

#endif // TEXTDISPLAY_H#endif