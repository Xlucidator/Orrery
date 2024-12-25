
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