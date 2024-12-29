#ifndef WORLD_H
#define WORLD_H

#include "common.h"
#include "world/Object.h"
#include "camera/Camera.h"
#include "shader/Shader.h"
#include "object/Model.h"

#include <vector>

class World {
public:
	

	World();
	~World() = default;

	void update();
	void render();

private:
	/*=== Object ===*/
	std::vector<Object> _objects;

	/*=== Camera ===*/
	Camera _camera;
	
	/*=== Light ===*/
	glm::vec3 _light_pos;
	glm::vec3 _light_color

	/*=== Time ===*/
	float _last_frame;
	float _delta_time;

	/*=== MVP ===*/
	glm::mat4 _model, _view, _projection;
	glm::mat3 _norm_model;

	/*=== Shader ===*/
	std::shared_ptr<Shader> _global_shader;

	void initPhysics();
	void initObjects();
};


#endif // !WORLD_H

