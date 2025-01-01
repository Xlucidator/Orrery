#ifndef ANIMDATA_H
#define ANIMDATA_H

#include <glm/glm.hpp>

struct BoneInfo {
	int id = -1; // index in bone matrices
	glm::mat4 offset; // transfer vertex from model space -> bone space

	BoneInfo() = default;
	BoneInfo(int id, glm::mat4& offset) : id(id), offset(offset) {}
};

#endif // !ANIMDATA_H