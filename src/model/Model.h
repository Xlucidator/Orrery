#ifndef MODEL_H
#define MODEL_H

#include "common.h"
#include "Mesh.h"
#include "shader/Shader.h"
#include "animation/animdata.h"

#include <string>
#include <vector>
#include <map>

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

GLuint textureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model {
public:
	Model(std::string path) { load(path); }
	~Model() = default;

	void draw(Shader* shader);

	/* Bone */
	auto& getBoneInfoMap() { return _bone_info_map; }
	int& getBoneCnt() { return _bone_cnt; }

	/* Physics */
	std::vector<physx::PxVec3> px_combined_vertices;
	std::vector<physx::PxU32>  px_combined_indices;

private:
	std::vector<Mesh> _meshes;
	std::vector<Texture> _textures_loaded;
	std::string _directory;

	void load(std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene); // TODO: maybe std::move && would better?
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string type_name);

	/* Bone */
	std::map<std::string, BoneInfo> _bone_info_map;
	int _bone_cnt = 0;
	void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
	void setVertexBoneData(Vertex& vertex, int boneid, float weight);

	/* Physics */
	void setPxCombinedMesh();
};


#endif // !MODEL_H
