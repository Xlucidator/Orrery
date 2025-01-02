#ifndef UTILS_H
#define UTILS_H


#include <assert.h>
#include <unordered_map>

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



glm::mat4 createModelMatrix(const glm::vec3 & position, const glm::vec3 & scale = glm::vec3(1.0f));
//inline glm::mat4 createModelMatrix(const glm::vec3 & position, const glm::vec3 & rotation, const glm::vec3 & scale = glm::vec3(1.0f));
glm::mat4 createModelMatrix(const glm::vec3 & position, const glm::vec3& front, const glm::vec3& up, const glm::vec3& right, const glm::vec3 & scale);
glm::mat4 createModelMatrix(const glm::vec3 & position, const glm::quat & rotation, const glm::vec3 & scale = glm::vec3(1.0f));


extern std::unordered_map<int8_t, glm::vec3> pace_vec;

#endif // !UTILS_H