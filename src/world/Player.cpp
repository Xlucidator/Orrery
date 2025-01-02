
#include "Player.h"


void Player::update(float _delta_time) {
	updateModelMatrix();
	if (_status == PLAYER_WALK) {
		animator->update(_delta_time);
	}
}


void Player::processKeyboard(float delta_time) {
	/* position and rotation */
	float velocity = _movement_speed * delta_time;
	// TODO: more direction
	if (keyboard[GLFW_KEY_W]) {
		_yaw = -90.0f;
	} 
	else if (keyboard[GLFW_KEY_D]) {
		_yaw = 0.0f;
	}
	else if (keyboard[GLFW_KEY_S]) {
		_yaw = 90.0f;
	}
	else if (keyboard[GLFW_KEY_A]) {
		_yaw = 180.0f;	
	}
	updateFront();
	_position += _front * velocity;
}

void Player::processMouseMovement(float xoffset, float yoffset) {

}

void Player::updateFront() {
	_front.x = cos(glm::radians(_yaw));
	_front.z = sin(glm::radians(_yaw));
}