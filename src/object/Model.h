#ifndef MODEL_H
#define MODEL_H

#include "common.h"
#include "Mesh.h"
#include "shader/Shader.h"

#include <string>
#include <vector>

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

GLuint textureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model {
public:
	Model(std::string& path) { load(path); }
	Model(std::string path) { load(path); }
	~Model() {} 

	void draw(Shader* shader);

private:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures_loaded;
	std::string directory;

	void load(std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene); // TODO: maybe std::move && would better?
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string type_name);
};


#endif // !MODEL_H
