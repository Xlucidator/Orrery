#ifndef UTILS_H
#define UTILS_H


#include <assert.h>

#include "common.h"

/* Error Check */
#ifdef DEBUG

#define GL_CALL(glfunc) do { \
	glfunc; \
	check_error(); } while (false)

#else
	
#define GL_CALL(glfunc) glfunc;

#endif

void check_error();


/* Define */
#define setVAO(vao, operation)  do { \
		GL_CALL(glBindVertexArray(vao)); \
			operation  \
		GL_CALL(glBindVertexArray(0));   \
	} while (false)


//physx::PxTransform

glm::mat4 createModelMatrix(const glm::vec3 & position, const glm::vec3 & scale = glm::vec3(1.0f));
//inline glm::mat4 createModelMatrix(const glm::vec3 & position, const glm::vec3 & rotation, const glm::vec3 & scale = glm::vec3(1.0f));
glm::mat4 createModelMatrix(const glm::vec3 & position, const glm::quat & rotation, const glm::vec3 & scale = glm::vec3(1.0f));


#endif // !UTILS_H