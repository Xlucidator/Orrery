#ifndef OBJECT_H
#define OBJECT_H

#include "common.h"
#include "shader/Shader.h"
#include "object/Model.h"

#include <string>

enum OBJType {
	MODEL = 0,
	SIMPLE
};

class Object {
public:
	Object(std::shared_ptr<Model> model, std::shared_ptr<Shader> shader, glm::mat4 model_matrix = glm::mat4(1.0f));
	~Object() = default;

	void render(glm::vec3& view, glm::vec3& projection); // TODO
	void draw(std::shared_ptr<Shader>& shader);

	void setModelMatrix(glm::mat4 model) { 
		_model_matrix = model;
		updateNormModelMatrix();
	}
	glm::mat4 getModelMatrix() { return _model_matrix; }
	glm::mat3 getNormModelMatrix() { return _norm_model_matrix; }

private:
	OBJType _type;
	std::shared_ptr<Shader> _shader = nullptr; // shared
	std::shared_ptr<Model> _model = nullptr;   // shared
	
	glm::mat4 _model_matrix;
	glm::mat3 _norm_model_matrix;
	// in details 
	glm::vec3 _position;
	float _scale;
	float _rotate_angle; // only rotate with y axis: world up, TODO:better
	
	inline void updateNormModelMatrix();
};


#endif // !OBJECT_H
