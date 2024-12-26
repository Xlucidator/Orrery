#ifndef SHADER_H
#define SHADER_H

#include "common.h"
#include <string>
//#include <iostream>
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>

class Shader {
public:
	Shader(const char* vertex_path, const char* fragment_path);
	~Shader();

	void begin(); // begin to use shader
	void end();   // end to use shader

	void setFloat(const std::string& name, float value);
	void setVec3f(const std::string& name, float x, float y, float z);
	void setVec3f(const std::string& name, const float* value);

private:
	GLuint _program = 0;

	void checkShaderErrors(GLuint target, int type); // type = 0: compile error, type = 1: link error

};

#endif // !SHADER_H
