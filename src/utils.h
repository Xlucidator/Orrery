#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <assert.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef DEBUG

#define GL_CALL(glfunc) do { \
	glfunc; \
	check_error(); } while (false)

#else
	
#define GL_CALL(glfunc) glfunc;

#endif

void check_error();

#endif // !UTILS_H