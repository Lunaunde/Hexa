#include"shader.h"
#include"../wrapper/GLErrorCheck.h"
#include<string>
#include<sstream>
#include<fstream>
#include <iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	//声明装入shader代码的string
	std::string vertexCode;
	std::string fragmentCode;
	//声明用于读取文件的流
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	//确保ifstream对象可以抛出异常
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		//打开文件
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		//创建文件流
		std::stringstream vShaderStream, fShaderStream;
		//读取文件的缓冲内容到数据流中
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//关闭文件处理器
		vShaderFile.close();
		fShaderFile.close();
		//转换数据流到string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR: Shader File Error: " << e.what() << std::endl;
	}

	const char* vertexShaderSource = vertexCode.c_str();
	const char* fragmentShaderSource = fragmentCode.c_str();
	GLuint vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertex, 1, &vertexShaderSource, NULL);
	glShaderSource(fragment, 1, &fragmentShaderSource, NULL);


	glCompileShader(vertex);//编译shader
	checkCompileErrors(vertex, "COMPILE");
	glCompileShader(fragment);
	checkCompileErrors(fragment, "COMPILE");

	mProgram = glCreateProgram();

	glAttachShader(mProgram, vertex);
	glAttachShader(mProgram, fragment);

	glLinkProgram(mProgram);

	checkCompileErrors(mProgram, "LINK");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
Shader::~Shader() 
{
	if (mProgram != 0) 
	{
		GL_CALL(glDeleteProgram(mProgram));
		mProgram = 0;
	}
}

void Shader::begin()
{
	GL_CALL(glUseProgram(mProgram));
}

void Shader::end()
{
	GL_CALL(glUseProgram(0));
}

void Shader::setFloat(const std::string& name, float value)
{
	GLuint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	GL_CALL(glUniform1f(location, value));
}
void Shader::setFloat3(const std::string& name, float v1, float v2, float v3)
{
	GLuint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

    GL_CALL(glUniform3f(location, v1, v2, v3));
}
void Shader::setInt(const std::string& name, int value)
{
	GLuint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	GL_CALL(glUniform1i(location, value));
}
void Shader::setUint(const std::string& name, unsigned int value)
{
	GLuint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	GL_CALL(glUniform1ui(location, value));
}
void Shader::setVec3(const std::string& name, float v1, float v2, float v3)
{
	GLuint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	GLfloat vec3[3] = { v1,v2,v3 };
	GL_CALL(glUniform3fv(location, 1, vec3));
}
void Shader::setMat4(const std::string& name, const float values[16])
{
    GLuint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

    GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, values));
}

void Shader::checkCompileErrors(GLuint target, std::string type)
{
	int success = 0;
	char infoLog[1024];

	if (type == "COMPILE")
	{
		glGetShaderiv(target, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(target, 1024, NULL, infoLog);
			std::cout << "Error: Shader compile error" << "\n" << infoLog << std::endl;
		}
	}
	else if (type == "LINK")
	{
		glGetProgramiv(target, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(target, 1024, NULL, infoLog);
			std::cout << "Error: Shader link error " << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		std::cout << "Error: Check shader errors type is wrong" << std::endl;
	}
}