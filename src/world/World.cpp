
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
}

void World::init() {
	initObjects();
}

void World::update() {
	/* Calculate time */
	float current_frame = static_cast<float>(glfwGetTime());
	_delta_time = current_frame - _last_frame;
	_last_frame = current_frame;

	/* React to Input */
	processKeyboardInput();
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
	// Load Shader & Model
	_global_shader = std::make_shared<Shader>(
		"assets/shaders/loadobj_nolight.vert", 
		"assets/shaders/loadobj_nolight.frag"
	);
	auto backpack = std::make_shared<Model>("assets/objects/backpack/backpack.obj");

	// Create Objects
	glm::mat4 model;
	glm::vec3 location = glm::vec3(2.0f, 5.0f, -15.0f);
	_objects.emplace_back(_global_shader, backpack);
	_objects.emplace_back(_global_shader, backpack, glm::translate(model, location));
}

void World::initPhysics() {
	std::cout << "Init PhysX" << std::endl;

	/* Initialize PhysX */
	// Create Foundation
	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mAllocator, mErrorCallback);
	if (!mFoundation) throw("PxCreateFoundation failed!");

	// Create PVD
	mPvd = PxCreatePvd(*mFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	// Creat Physics with Tolerance Scale
	// mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), true, mPvd); // Without
	mToleranceScale.length = 100; // typical length of an object
	mToleranceScale.speed = 981; // typical speed of an object, gravity*1s is a reasonable choice
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPvd);
	// mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale); // do not use PVD

	// Creat SceneDesc
	physx::PxSceneDesc scene_desc(mPhysics->getTolerancesScale());
	scene_desc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

	// Create CPU Dispatcher
	mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	scene_desc.cpuDispatcher = mDispatcher;
	scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;

	// Create Scene
	mScene = mPhysics->createScene(scene_desc);

	// Pvd Client
	physx::PxPvdSceneClient* pvd_client = mScene->getScenePvdClient();
	if (pvd_client) {
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	/* Create Simulation */
	mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f); // static friction, dynamic friction, restitution
	physx::PxRigidStatic* ground_plane = physx::PxCreatePlane(*mPhysics, physx::PxPlane(0.0f, 1.0f, 0.0f, 1.0f), *mMaterial);
	mScene->addActor(*ground_plane);
	//boxes.push_back(ground_plane);
}


/*=== Interact ===*/
void World::processKeyboardInput() {
	if (keyboard[GLFW_KEY_W]) _camera.processKeyboard(FORWARD, _delta_time);
	if (keyboard[GLFW_KEY_S]) _camera.processKeyboard(BACKWARD, _delta_time);
	if (keyboard[GLFW_KEY_A]) _camera.processKeyboard(LEFT, _delta_time);
	if (keyboard[GLFW_KEY_D]) _camera.processKeyboard(RIGHT, _delta_time);
}

void World::processMouseMovement(float xoffset, float yoffset) {
	_camera.processMouseMovement(xoffset, yoffset, true);
}

void World::processMouseScroll(float yoffset) {
	_camera.processMouseScroll(yoffset);
}