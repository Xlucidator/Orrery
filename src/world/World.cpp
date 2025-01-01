
#include "World.h"
#include "utils.h"

World::World() {
	/* Init Time */ 
	_last_frame = 0.0f;
	_delta_time = 0.0f;

	/* Init Camera */
	_camera = Camera(glm::vec3(0.0f, 0.0f, 10.0f));

	/* Init Light */
	_light_pos = glm::vec3(1.2f, 1.0f, 2.0f);
	_light_color = glm::vec3(1.0f);
}

void World::init() {
#ifdef PHYSIC_IMPL
	initPhysics();
#endif
	initObjects();
}

void World::update() {
	/* Calculate time */
	float current_frame = static_cast<float>(glfwGetTime());
	_delta_time = current_frame - _last_frame;
	_last_frame = current_frame;

	/* React to Input */
	processKeyboardInput();

	/* Animate */
	for (auto& obj : _objects) {
		obj.animator->update(_delta_time);
	}

	/* Get Physcs Simulation */
#ifdef PHYSIC_IMPL
	if (mScene) {
		mScene->simulate(_delta_time);
		mScene->fetchResults(true);
		for (auto& obj : _objects) {
			obj.updateSimulateResult();
		}
	}
#endif
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
		//_model = obj.getModelMatrix();
		//_global_shader->setMat4f("model", glm::value_ptr(_model));
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
	//auto backpack = std::make_shared<Model>("assets/objects/backpack/backpack.obj");
	auto barrel = std::make_shared<Model>("assets/objects/barrel/Barrel.obj");
	auto box = std::make_shared<Model>("assets/objects/box/box_resize.obj");
	auto barrels = std::make_shared<Model>("assets/objects/barrelpack/barrels_packed.obj");
	auto vampire = std::make_shared<Model>("assets/objects/vampire/dancing_vampire.dae");

	// Create Objects
	glm::mat4 model_transform[] = {
		// Location					 // Scale
		createModelMatrix(glm::vec3(-1.0f, 0.0f, 0.0f)),
		createModelMatrix(glm::vec3(3.0f, 0.0f, -4.0f)),
		createModelMatrix(glm::vec3(2.0f, 0.0f, -6.0f)),
		createModelMatrix(glm::vec3(-2.0f, 0.0f, -1.0f), glm::vec3(0.02f))
	};
	_objects.emplace_back(_global_shader, barrel, model_transform[0]);
	_objects.emplace_back(_global_shader, box   , model_transform[1]);
	_objects.emplace_back(_global_shader, barrels, model_transform[2]);
	_objects.emplace_back(_global_shader, vampire, model_transform[3]);

	// init Objects Physics
#ifdef PHYSIC_IMPL
	auto barrel_actor = _objects[0].createRigidDynamic(mPhysics, *mCookingParams, mMaterial);
	mScene->addActor(*barrel_actor);
#endif
}

void World::initPhysics() {
	std::cout << "Init PhysX" << std::endl;

	/* Initialize PhysX */
	// Create Foundation
	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mAllocator, mErrorCallback);
	if (!mFoundation) throw("PxCreateFoundation failed!");

	// Create PVD
	//mPvd = PxCreatePvd(*mFoundation);
	//physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	//mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	// Creat Physics with Tolerance Scale
	// mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), true, mPvd); // Without
	mToleranceScale.length = 100; // typical length of an object
	mToleranceScale.speed = 981; // typical speed of an object, gravity*1s is a reasonable choice
	// mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPvd);
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale); // do not use PVD

	// Init PxCooking
	mCookingParams = new physx::PxCookingParams(mPhysics->getTolerancesScale());
	// mCookingParams->meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eWELD_VERTICES

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
	//physx::PxPvdSceneClient* pvd_client = mScene->getScenePvdClient();
	//if (pvd_client) {
	//	pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	//	pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	//	pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	//}

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