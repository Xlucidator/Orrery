#ifndef WORLD_H
#define WORLD_H

#include "common.h"
#include "world/Object.h"
#include "camera/Camera.h"
#include "shader/Shader.h"
#include "model/Model.h"

#include <vector>

class World {
public:
	World();
	~World() {
		if (mCookingParams) delete mCookingParams;
		if (mMaterial) mMaterial->release();
		if (mScene) mScene->release();
		if (mPhysics) mPhysics->release();
		if (mFoundation) mFoundation->release();
		if (mDispatcher) mDispatcher->release();
	}

	void init();
	void update();
	void render();

	/*=== Interact ===*/
	void processKeyboardInput();
	void processMouseMovement(float xoffset, float yoffset);
	void processMouseScroll(float yoffset);

private:
	/*=== Object ===*/
	std::vector<Object> _objects;

	/*=== Camera ===*/
	Camera _camera;

	/*=== Light ===*/
	glm::vec3 _light_pos;
	glm::vec3 _light_color;

	/*=== Time ===*/
	float _last_frame = 0.0f;
	float _delta_time = 0.0f;

	/*=== MVP ===*/
	glm::mat4 _model, _view, _projection;
	glm::mat3 _norm_model;

	/*=== Shader ===*/
	std::shared_ptr<Shader> _global_shader;

	/*=== Physics ===*/
	// PhysX Variables
	physx::PxDefaultAllocator		mAllocator;
	physx::PxDefaultErrorCallback	mErrorCallback;
	physx::PxDefaultCpuDispatcher*	mDispatcher = nullptr;
	physx::PxTolerancesScale		mToleranceScale;

	physx::PxFoundation*			mFoundation = nullptr;
	physx::PxPhysics*				mPhysics = nullptr;
	
	physx::PxScene*					mScene = nullptr;
	physx::PxMaterial*				mMaterial = nullptr;
	physx::PxCookingParams*			mCookingParams = nullptr;  // no more PxCooking
	
	//physx::PxPvd*					mPvd = nullptr;

	void initPhysics();
	void initObjects();
};


#endif // !WORLD_H

