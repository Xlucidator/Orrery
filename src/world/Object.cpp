
#include "Object.h"



Object::Object(std::shared_ptr<Model> model, std::shared_ptr<Shader> shader, glm::mat4 model_matrix) {
	_type = MODEL;
	_model = model;
	_shader = shader;

	_model_matrix = model_matrix; // glm::mat4(1.0f);
	updateNormModelMatrix();

	_position = glm::vec3(0.0f);
	_scale = 1.0f;
	_rotate_angle = 0.0f;
}


void Object::draw(std::shared_ptr<Shader>& shader) {
	_model->draw(shader.get());
}

void Object::render(glm::vec3& view, glm::vec3& projection) {
	//_shader->begin();

	// TODO: whether to render here or out in the world
	_model->draw(_shader);

	//_shader->end();
}

inline void Object::updateNormModelMatrix() {
	_norm_model_matrix = glm::transpose(glm::inverse(glm::mat3(_model_matrix)));
}
