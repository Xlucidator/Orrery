
#include "utils.h"

void check_error() {
	GLenum error_code = glGetError();
	if (error_code == GL_NO_ERROR) return;
	
	std::string error_str = "";
	switch (error_code) {
	case GL_INVALID_ENUM :	error_str = "GL_INVALID_ENUM";	break;
	case GL_INVALID_VALUE:	error_str = "GL_INVALID_VALUE"; break;
	case GL_INVALID_OPERATION: error_str = "GL_INVALID_OPERATION";  break;
	case GL_OUT_OF_MEMORY:	error_str = "GL_OUT_OF_MEMORY"; break;
	default: 
		error_str = "UNKNOWN ERROR";
		break;
	}
	std::cout << error_str << std::endl;
	assert(false);
}

glm::mat4 createModelMatrix(const glm::vec3& position, const glm::vec3& scale) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, scale);
	return model; // model = translate (* rotate) * scale
}

//glm::mat4 createModelMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
//	glm::mat4 model = glm::mat4(1.0f);
//	// translate
//	model = glm::translate(model, position);
//	// rotate
//	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // around x axis
//	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // around y axis
//	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // around z axis
//	// scale
//	model = glm::scale(model, scale);
//	return model; // // model = translate * rotate * scale
//}

glm::mat4 createModelMatrix(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = model * glm::mat4_cast(rotation);
	model = glm::scale(model, scale);
	return model; // // model = translate * rotate * scale
}