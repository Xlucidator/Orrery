#ifndef BONE_H
#define BONE_H

#include "common.h"

#include <vector>
#include <string>
#include <algorithm>

struct KeyPosition {
	glm::vec3 position;
	float timestamp;
};

struct KeyRotation {
	glm::quat rotation;
	float timestamp;
};

struct KeyScale {
	glm::vec3 scale;
	float timestamp;
};


class Bone {
public:
	Bone(const std::string& name, int id, const aiNodeAnim* channel);
	~Bone() = default;

	void update(float animation_time);

	glm::mat4 getLocalTransform() const { return _local_transform; }
	std::string getBoneName() const { return _name; }
	int getBoneId() const { return _id; }

	int getPositionIndex(float animation_time) {
		return findKeyFrameIndex(_positions, animation_time);
	}
	int getRotationIndex(float animation_time) {
		return findKeyFrameIndex(_rotations, animation_time);
	}
	int getScaleIndex(float animation_time) {
		return findKeyFrameIndex(_scales, animation_time);
	}

private:
	int _id;
	std::string _name;
	glm::mat4 _local_transform;

	std::vector<KeyPosition> _positions;
	std::vector<KeyRotation> _rotations;
	std::vector<KeyScale> _scales;
	int _pos_num = 0, _rot_num = 0, _scl_num = 0;

	inline float getScaleFactor(float last_timestamp, float next_timestamp, float animation_time);
	glm::mat4 interpolatePosition(float animation_time);
	glm::mat4 interpolateRotation(float animation_time);
	glm::mat4 interpolateScaling(float animation_time);

	/* Find the index i in keyFrames that satisfies:
	 *  keyFrames[i].timestamp <= animation_time < keyFrames[i+1].timestamp
	 *  - empty keyFrames: return 0
	 *  - equals to : 
	 *		int i;
	 *      for (i = 0; i < _scl_num - 1; i++)
	 *		    if (animation_time < _scales[i + 1].timestamp) return i;
	 *		return i;
	 */ template<typename T>
	int findKeyFrameIndex(const std::vector<T>& keyFrames, float time) {
		if (keyFrames.empty() || time < keyFrames.front().timestamp) return 0;

		auto it = std::upper_bound(keyFrames.begin(), keyFrames.end(), time, [](float t, const T& frame){ 
			return t < frame.timestamp; 
		});
		return it - keyFrames.begin() - 1;
	}
};


struct BoneInfo {
	int id = -1; // index in bone matrices
	glm::mat4 offset; // transfer vertex from model space -> bone space

	BoneInfo() = default;
	BoneInfo(int id, glm::mat4& offset) : id(id), offset(offset) {}
};


#endif // !BONE_H
