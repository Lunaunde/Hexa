#pragma once

#include "core.h"
#include <string>

class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();

	void begin();

	void end();
	void setFloat(const std::string& name, float value);
	void setInt(const std::string& name, int value);
	void setUint(const std::string& name, unsigned int value);
	void setVec3(const std::string& name, float v1,float v2,float v3);
private:
	//shader program
	//type: COMPILE, LINK
	void checkCompileErrors(GLuint target, std::string type);

private:
	GLuint mProgram{ 0 };
};