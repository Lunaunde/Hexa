#include "GLErrorCheck.h"
#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<string>
#include<assert.h>
void GLErrorCheck()
{
	std::string errorType;
	GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
	{
		switch (errorCode)
		{
		case GL_INVALID_ENUM:
			errorType = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			errorType = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			errorType = "GL_INVALID_OPERATION";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			errorType = "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:
			errorType = "GL_OUT_OF_MEMORY";
			break;
		default:
			errorType = "UNKNOWN_ERROR";
		}
		std::cout << "OpenGL错误:" << errorType << std::endl;
		assert(false);//程序断死
	}
}