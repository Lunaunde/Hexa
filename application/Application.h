#ifndef APPLICATION_H
#define APPLICATION_H
#define aplct Application::getInstance()

struct GLFWwindow;

using ResizeCallback = void(*)(int width, int lenght);
using KeyCallback = void(*)(int key, int scancode, int action, int mods);
using CursorPosCallback = void(*)(double xpos, double ypos);
using MouseButtonCallback = void(*)(int button, int action, int mods);
using ScrollCallback = void(*)(double xoffset, double yoffset);

class Application
{
public:
	~Application();

	static Application* getInstance();

	bool init(const unsigned int& width = 1920, const unsigned int& lenght = 1080);//1280 720 1920 1080
	bool update();
	void destroy();

	unsigned int getWidth()const { return mWidth; }
	unsigned int getLength()const { return mLenght; }

	void setResizeCallback(ResizeCallback callback) { mResizeCallback = callback; }
	void setKeyCallback(KeyCallback callback) { mKeyCallback = callback; }
	void setCursorPosCallback(CursorPosCallback callback) { mCursorPosCallback = callback; }
	void setMouseButtonCallback(MouseButtonCallback callback) { mMouseButtonCallback = callback; }
    void setScrollCallback(ScrollCallback callback) { mScrollCallback = callback; }

private:
	static void frameBufferSizeCallback(GLFWwindow* window,int width ,int lenght);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

private:
	static Application* instance;

	unsigned int mWidth;
	unsigned int mLenght;
	GLFWwindow* mWindow{ nullptr };

    ResizeCallback mResizeCallback{ nullptr };
	KeyCallback mKeyCallback{ nullptr };
	CursorPosCallback mCursorPosCallback{ nullptr };
    MouseButtonCallback mMouseButtonCallback{ nullptr };
    ScrollCallback mScrollCallback{ nullptr };

	Application();
};

#endif