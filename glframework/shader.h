#ifndef SHADER_H
#define SHADER_H

#include "core.h"
#include <string>

class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();

	void begin();

	void end();
	void setFloat(const std::string& name, const  float value);
	void setFloat3(const std::string& name, const  float v1, const  float v2, const  float v3);
	void setInt(const std::string& name, const  int value);
	void setUint(const std::string& name, const  unsigned int value);
	void setVec3(const std::string& name, const  float v1, const float v2, const float v3);
	void setMat4(const std::string& name, const float values[16]);
private:
	//shader program
	//type: COMPILE, LINK
	void checkCompileErrors(GLuint target, std::string type);

private:
	GLuint mProgram{ 0 };
};

#endif