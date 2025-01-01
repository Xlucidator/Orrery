#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "common.h"
#include "Animation.h"

#include <vector>


#define ANIMATION_MAX_BONES 100

class Animator {
public:
	Animator(Animation* animation);
	~Animator() = default;

	void init(Animation* played_animation);
	void update(float delta_time);
	

	void calculateBoneTransformRecursive(const AssimpNodeData* node, glm::mat4 parent_transform);
	
	const std::vector<glm::mat4>& getFinalBoneMatrices() { return _final_bone_matrices; }

private:
	std::vector<glm::mat4> _final_bone_matrices;
	Animation* _current_animation;
	float _current_time = 0.0f;
	float _delta_time = 0.0f;
};

#endif // !ANIMATOR_H
