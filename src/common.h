#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <memory>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // for print

extern uint32_t viewport_width;
extern uint32_t viewport_height;

#endif // !COMMON_H
