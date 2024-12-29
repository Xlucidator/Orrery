#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
    /* Camera Attribute */
    glm::vec3 position;
	glm::vec3 front, up, right;
	glm::vec3 world_up;
	/* Euler Angles */
    float yaw;
    float pitch;
	/* Camera options */
	float movement_speed;
	float mouse_sensitivity;
	float zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 wup = glm::vec3(0.0f, 1.0f, 0.0f), 
		   float yaw = YAW, float pitch = PITCH);
	~Camera();

	glm::mat4 getViewMatrix() { return glm::lookAt(position, position + front, up); }

	void processKeyboard(Movement direction, float deltaTime);
	void processMouseMovement(float xoffset, float yoffset, GLboolean constrain_pitch = true);
	void processMouseScroll(float yoffset);

private:
	void update(); // update camera vector
};

#endif // !CAMERA_H
