
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Model::draw(Shader* shader) {
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i].draw(shader);
	}
}

void Model::load(std::string& path) {
	//stbi_set_flip_vertically_on_load(true); // reverse texture picture y axis
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs/* | aiProcess_GenNormals*/);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}


/* Processing the Data Structure */
// traverse in nodes to retain the relationship between meshes, for further usaged if necessary
void Model::processNode(aiNode* node, const aiScene* scene) {
	for (int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.emplace_back(processMesh(mesh, scene));
	}
	for (int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	/* Get Vertices */
	for (int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.normal = (mesh->HasNormals()) ?  // notice: could have no normals (currently)
			glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : glm::vec3(0.0f);
		vertex.texcoord = (mesh->mTextureCoords[0]) ?
			glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f);
		vertices.push_back(vertex);
	}

	/* Get Indices */
	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	/* Get Textures */
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// diffuse texture
		std::vector<Texture> diffuse_maps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
		// specular texture
		std::vector<Texture> specular_maps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
		// normal texture
		std::vector<Texture> normal_maps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string type_name) {
	std::vector<Texture> textures;
	
	int texture_cnt = mat->GetTextureCount(type);
	bool skip = false;
	for (int i = 0; i < texture_cnt; i++) {
		aiString str;
		mat->GetTexture(type, i, &str); // get texture path -> str

		skip = false;
		for (int j = 0; j < textures_loaded.size(); j++) {
			// simple compare
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true; // path is the same, so this one has loaded yet -> skip
				break;
			}
		}
		if (skip) continue;

		Texture texture;
		texture.id = textureFromFile(str.C_Str(), directory);
		texture.type = type_name;
		texture.path = str.C_Str();
		textures.push_back(texture);
		textures_loaded.push_back(texture);
	}
	return textures;
}

GLuint textureFromFile(const char* path, const std::string& directory, bool gamma) {
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	/* Init Texture Object: texture_id */
	GLuint texture_id;
	glGenTextures(1, &texture_id);

	/* Loaded Texture Picture */
	int width, height, component_num;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &component_num, 0);
	if (!data) { // loaded failed
		std::cout << "Failed to load texture at path: " << path << std::endl;
		stbi_image_free(data);
		return texture_id;
	}

	glBindTexture(GL_TEXTURE_2D, texture_id);
	
	/* Send Data to Texture Object */
	GLenum format = (component_num == 1) ? GL_RED : (component_num == 3 ? GL_RGB : GL_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	/* Texture Filtering Settings */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	/* Mipmap Settings */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	stbi_image_free(data);

	return texture_id;
}