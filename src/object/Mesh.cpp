
#include "Mesh.h"
#include "utils.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures):
			vertices(vertices), indices(indices), textures(textures) {
	setup();
}

Mesh::~Mesh() {

}

void Mesh::draw(Shader* shader) {
	/* Set Texture Units */
	uint32_t diffuse_num = 1; // at least 16, type can be smaller
	uint32_t specular_num = 1;
	uint32_t normal_num = 1;
	for (int i = 0; i < textures.size(); i++) {
		GL_CALL(glActiveTexture(GL_TEXTURE0 + i)); // activate texture unit i
		
		// set the sampler to the correct texture unit
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")	number = std::to_string(diffuse_num++);
		else if (name == "texture_specular") number = std::to_string(specular_num++);
		else if (name == "texture_normal") number = std::to_string(normal_num++);
		shader->setInt("material." + name + number, i);  // bind "material.texture_xxn" sampler (sample2D) to texture unit i

		GL_CALL(glBindTexture(GL_TEXTURE_2D, textures[i].id)); // bind to textures[i] to texture unit i
	}
	GL_CALL(glActiveTexture(GL_TEXTURE0)); // reset to default

	/* Draw Mesh */
	glBindVertexArray(_vao);
	GL_CALL(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));
	glBindVertexArray(0);
}

void Mesh::setup() {
	/* Create VAO, VBO and EBO */
	GL_CALL(glGenVertexArrays(1, &_vao));
	GL_CALL(glGenBuffers(1, &_vbo));
	GL_CALL(glGenBuffers(1, &_ebo));

	/* Bind VAO */
	setVAO(_vao, {
		/*=== init vbo and ebo data ===*/
		// bind vbo and send data
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW));
		// bind ebo and send data
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo));
		GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW));
		
		/*=== connect vbo and ebo with vao ===*/
		// vertex position
		GL_CALL(glEnableVertexAttribArray(0));
		GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
		// vertex normal
		GL_CALL(glEnableVertexAttribArray(1));
		GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)));
		// vertex texture coordinate
		GL_CALL(glEnableVertexAttribArray(2));
		GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord)));
	});
}