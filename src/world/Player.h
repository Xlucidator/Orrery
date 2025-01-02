#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"
#include "Object.h"

enum PlayerStatus {
	PLAYER_IDLE = 0,
	PLAYER_WALK,
};

class Player : public Object {
public:
	Player() = default;
	Player(std::shared_ptr<Shader> shader, std::shared_ptr<Model> model, glm::mat4 model_matrix = glm::mat4(1.0f))
		: Object(shader, model, model_matrix) { }
	~Player() {}

	inline void walk() { _status = PLAYER_WALK; }
	inline void idle() { _status = PLAYER_IDLE; animator->reset(); }
	void update(float delta_time) override;

	void processKeyboard(float delta_time) override;
	void processMouseMovement(float xoffset, float yoffset) override;

	void updateModelMatrix() {
		_model_matrix = glm::translate(glm::mat4(1.0f), _position);
		_model_matrix = glm::rotate(_model_matrix, glm::radians(_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		//_model_matrix = glm::scale(_model_matrix, glm::vec3(0.01f));
		updateNormModelMatrix();
	}

protected:
	PlayerStatus _status = PLAYER_IDLE;
	float _movement_speed = 5.0f;

	glm::vec3 _front = glm::vec3(0.0f, 0.0f, 1.0f);
	float _yaw = 90.0f;
	void updateFront();
};

#endif // !PLAYER_H
