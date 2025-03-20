#include"Application.h"
#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include<iostream>

Application* Application::instance = nullptr;
Application* Application::getInstance()
{
	if (instance == nullptr)
	{
		instance = new Application();
	}
	return instance;
}

bool Application::init(const unsigned int& width, const unsigned int& lenght)
{
	mWidth = width;
	mLenght = lenght;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	mWindow = glfwCreateWindow(width, lenght, "OpenGLStudy", NULL, NULL);//(长,宽,窗口名,NULL,NULL)
	if (mWindow == NULL)
	{
		return false;
	}
	glfwMakeContextCurrent(mWindow);//指定绘制窗口

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))//使用glad加载所有当前版本的OpenGL函数
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	glfwSetFramebufferSizeCallback(mWindow, frameBufferSizeCallback);
	glfwSetKeyCallback(mWindow, keyCallback);
	glfwSetCursorPosCallback(mWindow, cursorPosCallback);
	glfwSetMouseButtonCallback(mWindow, mouseButtonCallback);
	glfwSetScrollCallback(mWindow, scrollCallback);
	glfwSetWindowUserPointer(mWindow, this);

	return true;
}
bool Application::update()
{
	if(glfwWindowShouldClose(mWindow))
        return false;

	glfwPollEvents();
	glfwSwapBuffers(mWindow);//切换双缓冲区
	return true;
}
void Application::destroy()
{
	glfwTerminate();
}

void Application::frameBufferSizeCallback(GLFWwindow* window, int width, int lenght)
{
	//std::cout << "Framebuffer size changed: " << width << ", " << lenght << std::endl;
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->mResizeCallback != nullptr)
		self->mResizeCallback(width, lenght);
	//if(aplct->mResizeCallback != nullptr)
	//	   aplct->mResizeCallback(width, lenght);
}

void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->mKeyCallback != nullptr)
		self->mKeyCallback(key, scancode, action, mods);
}

void Application::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->mCursorPosCallback != nullptr)
		self->mCursorPosCallback(xpos, ypos);
}

void Application::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->mMouseButtonCallback != nullptr)
		self->mMouseButtonCallback(button, action, mods);
}

void Application::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->mScrollCallback != nullptr)
		self->mScrollCallback(xoffset, yoffset);
}

Application::Application()
{
}
Application::~Application()
{
}