#ifndef ANIMATION_H
#define ANIMATION_H

#include "common.h"
#include "model/Bone.h"
#include "model/Model.h"

#include <vector>
#include <string>
#include <map>

struct AssimpNodeData {
	std::string name;
	glm::mat4 transformation;
	
	std::vector<AssimpNodeData> children;
	int child_num;

	AssimpNodeData() = default;
	AssimpNodeData(const std::string& name, const glm::mat4& transform, int child_num)
		:transformation(transform), name(name), child_num(child_num) {}
};


class Animation {
public:
	Animation() = default;
	Animation(const std::string& animation_path, Model& model);
	~Animation() = default;

	Bone* findBone(const std::string& name);

	inline float getDuration() { return _duration; }
	inline float getTicksPerSecond() { return _ticks_per_second; }
	inline const auto& getRootNode() { return _root; }
	inline const auto& GetBoneIDMap() { return _boneinfo_map; }

private:
	float _duration;
	int _ticks_per_second;
	std::vector<Bone> _bones;
	AssimpNodeData _root;
	std::map<std::string, BoneInfo> _boneinfo_map;

	//void readHierarchyData(AssimpNodeData& dest, const aiNode* src);
	AssimpNodeData traverseAiNodeRecursive(const aiNode* src);
	void readMissingBones(const aiAnimation* animation, Model& model);
};

#endif // !ANIMATION_H
