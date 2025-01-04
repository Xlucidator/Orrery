
#include "World.h"
#include "utils.h"

bool kinematicTouchStatic = false;

World::World() {
	/* Init Time */ 
	_last_frame = 0.0f;
	_delta_time = 0.0f;

	/* Init Camera */
	_camera = Camera(glm::vec3(0.0f, 11.0f, 10.0f));

	/* Init Light */
	_light_pos = glm::vec3(1.2f, 1.0f, 2.0f);
	_light_color = glm::vec3(1.0f);

}

void World::init() {
#ifdef PHYSIC_IMPL
	initPhysics();
#endif
	initObjects();

	/* Init BGM */
	sound_engine->play2D("assets/audios/Skeleton_A_Bloody_Labyrinth.mp3", true);
}

void World::start() {
#ifdef PHYSIC_IMPL
	for (auto& obj : _objects) {
		obj->start();
	}
#endif // PHYSIC_IMPL

	// important! or the first _delta_time would be extremely large
	_last_frame = static_cast<float>(glfwGetTime());
}

void World::update() {
	/* Calculate time */
	float current_frame = static_cast<float>(glfwGetTime());
	_delta_time = current_frame - _last_frame;
	_last_frame = current_frame;

	/* React to Input */
	processKeyboardInput();

	/* Object Update: Animate or Try Moving */
	for (auto& obj : _objects) {
		obj->update(_delta_time);
	}

	/* Get Physcs Simulation: Update or Amend */
#ifdef PHYSIC_IMPL
	if (mScene) {
		mScene->simulate(_delta_time);
		mScene->fetchResults(true);
		for (auto& obj : _objects) {
			obj->updateSimulateResult();
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
	// Use Global Shader for Now as a Makeshift
	_global_shader->begin();

	_global_shader->setMat4f("view", glm::value_ptr(_view));
	_global_shader->setMat4f("projection", glm::value_ptr(_projection));
	for (auto& obj : _objects) {
		obj->draw(_global_shader);
	}

	_global_shader->end();
}

void World::initObjects() {
	/* Load Shader& Model */
	_global_shader = std::make_shared<Shader>(
		"assets/shaders/loadobj_nolight.vert", 
		"assets/shaders/loadobj_nolight.frag"
	);

	auto ground = std::make_shared<Model>("assets/objects/ground/ground.obj");
	auto avatar = std::make_shared<Model>("assets/objects/darknight/darknight.fbx");
	
	auto barrel = std::make_shared<Model>("assets/objects/barrel/Barrel.obj");
	auto box = std::make_shared<Model>("assets/objects/box/box_resize.obj");
	auto barrels = std::make_shared<Model>("assets/objects/barrelpack/barrels_packed.obj");
	//auto vampire = std::make_shared<Model>("assets/objects/vampire/dancing_vampire.dae");
	auto knight = std::make_shared<Model>("assets/objects/knightguard/Knighty92-onlyman.fbx");
	//auto ruins = std::make_shared<Model>("assets/objects/ruins/Ruins-Pillars-Arch.obj");
	/* Debug */
	// knight->printMesh();

	/* Create Ground */
	float ground_interval = 30.0f;
	for (int i = -1; i <= 1; i++) { // _objects[0 ~ 8]
		for (int j = -1; j <= 1; j++) {
			_objects.emplace_back(std::make_shared<Object>(_global_shader, ground, STATIC,
				glm::vec3(i * ground_interval, 0.0f, j * ground_interval)
			));
		}
	}
	
	/* Create Player */
	_player = std::make_shared<Player>(_global_shader, avatar, _border); // Must be Dynamic
	_objects.push_back(_player);
	_camera.followAt(_player);

	/* Create Objects */
	glm::mat4 model_transform[] = {  // Location		// Scale
		createModelMatrix(glm::vec3(-1.0f, 2.0f,  0.0f)),
		createModelMatrix(glm::vec3( 3.0f, 0.0f, -4.0f)),
		createModelMatrix(glm::vec3( 2.0f, 0.0f, -8.0f)),
		createModelMatrix(glm::vec3(-5.0f, 0.0f, -5.0f))
	};

	_objects.emplace_back(std::make_shared<Object>(_global_shader, barrel, DYNAMIC,
		glm::vec3(-4.0f, 0.5f, 0.0f), 1.0f, glm::vec3(0.0f, -1.0f, 0.2f)
	));
	_objects.emplace_back(std::make_shared<Object>(_global_shader, box, STATIC,
		glm::vec3(3.0f, 0.0f, -4.0f), 1.0f
	));
	_objects.emplace_back(std::make_shared<Object>(_global_shader, barrels, model_transform[2], STATIC));
	_objects.emplace_back(std::make_shared<Object>(_global_shader, knight, model_transform[3], STATIC, 0.7f));

	/* init Objects Physics */
#ifdef PHYSIC_IMPL
	// Ground
	physx::PxRigidStatic* ground_plane = mPhysics->createRigidStatic(physx::PxTransform(0.0f, -0.5f, 0.0f));
	{
		physx::PxShape* ground_shape = mPhysics->createShape(physx::PxBoxGeometry(50.0f, 0.5f, 50.0f), *mMaterial);
		ground_plane->attachShape(*ground_shape);
		ground_shape->release();
	}
	_objects[4]->rigid_static = ground_plane; // ground in _objects[4] is loacated in (0.0f, 0.0f, 0.0f) 
	mScene->addActor(*ground_plane);

	// Player
	auto player = _player->createRigidDynamic(mPhysics, *mCookingParams, mMaterial);
	mScene->addActor(*player);
	
	// Other Objects
	auto barrel_actor = _objects[10]->createRigidDynamic(mPhysics, *mCookingParams, mMaterial);
	barrel_actor->setMass(5.0f);
	barrel_actor->setCMassLocalPose(physx::PxTransform(0.0f, 0.7f, 0.0f));
	mScene->addActor(*barrel_actor);

	auto box_actor = _objects[11]->createRigidStatic(mPhysics, *mCookingParams, mMaterial);
	mScene->addActor(*box_actor);

	for (int i = 10; i < _objects.size(); i++) {/* Other Objects in Batch */}
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

	// Set Filter Shader
	scene_desc.filterShader = MyFilterShader;

	// Create Scene
	mScene = mPhysics->createScene(scene_desc);
	mCollisionCallback = new MyCollisionCallback();
	mScene->setSimulationEventCallback(mCollisionCallback);

	// Pvd Client
	//physx::PxPvdSceneClient* pvd_client = mScene->getScenePvdClient();
	//if (pvd_client) {
	//	pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	//	pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	//	pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	//}

	/* Create Global Material */
	mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f); // static friction, dynamic friction, restitution
}

physx::PxFilterFlags MyFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize) {
	// enable default flags
	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
	// add touch notification
	pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
	pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
	return physx::PxFilterFlag::eDEFAULT;
}


/*=== Interact ===*/
void World::processKeyboardPress() {
	if (is_wasd()) {
		_player->walk();
	}

	// Debug: report postion
	if (keyboard[GLFW_KEY_SPACE]) {
		std::cout << "[Player Position] " << glm::to_string(_player->getPosition()) << std::endl;
	}
}

// Attension: This function is called successively every window refresh, not keyboard callback
void World::processKeyboardInput() {
	if (is_wasd()) {
		_camera.processKeyboard(_delta_time);
		_player->processKeyboard(_delta_time);
	}
}

void World::processKeyboardRelease() {
	if (!is_wasd()) {
		_player->idle();
	}
}

void World::processMouseMovement(float xoffset, float yoffset) {
	//_camera.processMouseMovement(xoffset, yoffset, true);
}

void World::processMouseScroll(float yoffset) {
	_camera.processMouseScroll(yoffset);
}