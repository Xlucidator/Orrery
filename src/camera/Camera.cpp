
#include "Camera.h"


Camera::Camera(glm::vec3 pos, glm::vec3 wup, float yaw, float pitch):
		position(pos), world_up(wup), yaw(yaw), pitch(pitch),
		front(glm::vec3(0.0f, 0.0f, -1.0f)), // reverse to direction
		movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM) {
	update();
}

Camera::~Camera() {

}


/* Camera Movement */
void Camera::processKeyboard(Movement direction, float delta_t) {
	float velocity = movement_speed * delta_t;
	switch (direction) {
		case FORWARD: position += front * velocity; break;
		case BACKWARD:position -= front * velocity; break;
		case LEFT:	  position -= right * velocity; break;
		case RIGHT:	  position += right * velocity; break;
		default: break;
	}
}

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrain_pitch) {
    xoffset *= mouse_sensitivity;
    yoffset *= mouse_sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    // Limited the bound of pitch
    if (constrain_pitch) {
        if (pitch >  89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }

    update();
}

void Camera::processMouseScroll(float yoffset) {
	zoom -= yoffset;
	if (zoom <  1.0f) zoom = 1.0f;
	if (zoom > 45.0f) zoom = 45.0f;
}



/* Camera Vector Update */ 
void Camera::update() {
	/* Calculate the new Front vector */
	glm::vec3 front;
	front.y = sin(glm::radians(pitch));
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

	/* Re-calculate the right and up vector, need normalized for usage in speed */
	this->front = glm::normalize(front);
	this->right = glm::normalize(glm::cross(this->front, this->world_up));
	this->up = glm::normalize(glm::cross(this->right, this->front));
}