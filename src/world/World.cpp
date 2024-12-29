
#include "World.h"
#include "utils.h"

World::World() {
	/* Init Time */ 
	_last_frame = 0.0f;
	_delta_time = 0.0f;

	/* Init Camera */
	_camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));

	/* Init Light */
	_light_pos = glm::vec3(1.2f, 1.0f, 2.0f);
	_light_color = glm::vec3(1.0f);

	/* Init Objects */
	initObjects();
}

void World::update() {
	/* Calculate time */
	float current_frame = static_cast<float>(glfwGetTime());
	_delta_time = current_frame - _last_frame;
	_last_frame = current_frame;

}

void World::render() {
	/* Clear canvas */
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	/* Global Peojection & View Matrix */
	_projection = glm::perspective(glm::radians(_camera.zoom), (float)viewport_width / (float)viewport_height, 0.1f, 100.0f);
	_view = _camera.getViewMatrix();

	/* For Each Object: Get Model Matrix and Shade */
	// Use Global Shader for now
	_global_shader->begin();

	_global_shader->setMat4f("view", glm::value_ptr(_view));
	_global_shader->setMat4f("projection", glm::value_ptr(_projection));
	for (auto& obj : _objects) {
		_model = obj.getModelMatrix();
		_global_shader->setMat4f("model", glm::value_ptr(_model));
		//_norm_model = obj.getNormModelMatrix();
		//_global_shader->setMat4f("normModel", glm::value_ptr(_norm_model));
		obj.draw(_global_shader);
	}

	_global_shader->end();
}

void World::initObjects() {
	// load shader & model
	_global_shader = std::make_shared<Shader>(
		"assets/shaders/loadobj_nolight.vert", 
		"assets/shaders/loadobj_nolight.frag"
	);
	auto backpack = std::make_shared<Model>("assets/objects/backpack/backpack.obj");

	// create objects
	_objects.emplace_back(_global_shader, backpack);
}

void World::initPhysics() {

}

