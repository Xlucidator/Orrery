
#include "Player.h"



void Player::walk() {
	_status = PLAYER_WALK; 
	if (!walking_sound || walking_sound->isFinished()) {
		walking_sound = sound_engine->play2D("assets/audios/walking.mp3", true, false, true);
		walking_sound->setVolume(0.8f);
	}
}

void Player::idle() { 
	_status = PLAYER_IDLE; 
	animator->reset(); 
	if (walking_sound && !walking_sound->isFinished()) {
		walking_sound->stop();
	}
}

void Player::update(float _delta_time) {
	updateModelMatrix();
	if (_status == PLAYER_WALK) {
		animator->update(_delta_time);
	}
}


void Player::processKeyboard(float delta_time) {
	/* position and rotation */
	float velocity = _movement_speed * delta_time;

	// be careful: assume true = 1, false = 0
	glm::vec3 pace = static_cast<float>(keyboard[GLFW_KEY_W]) * pace_vec[GLFW_KEY_W] 
				   + static_cast<float>(keyboard[GLFW_KEY_S]) * pace_vec[GLFW_KEY_S] 
				   + static_cast<float>(keyboard[GLFW_KEY_A]) * pace_vec[GLFW_KEY_A] 
				   + static_cast<float>(keyboard[GLFW_KEY_D]) * pace_vec[GLFW_KEY_D];
	
	if (pace != glm::vec3(0.0f, 0.0f, 0.0f)) {
		pace = glm::normalize(pace);
		_front = pace; // depend on createModelMactrix: front (not -front)
		_position += pace * velocity;
	}
}

void Player::processMouseMovement(float xoffset, float yoffset) {

}

void Player::processMouseScroll(float yoffset) {

}


physx::PxRigidDynamic* Player::createRigidDynamic(physx::PxPhysics* physics, physx::PxCookingParams& cookingParams, physx::PxMaterial* material) {
	cookAndCreateTriangleMesh(physics, cookingParams);

	px_type = DYNAMIC;
	_px_transform = physx::PxTransform(
		physx::PxVec3(_position.x, _position.y, _position.z),
		physx::PxQuat(_rotation.x, _rotation.y, _rotation.z, _rotation.w)
	);
	rigid_dynamic = physics->createRigidDynamic(_px_transform);
	{
		physx::PxShape* shape = physics->createShape(physx::PxTriangleMeshGeometry(px_triangle_mesh), *material);
		rigid_dynamic->attachShape(*shape);
		shape->release();
	}
	rigid_dynamic->setLinearDamping(0.01f);
	rigid_dynamic->setAngularDamping(0.5f);
	rigid_dynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
	rigid_dynamic->setSleepThreshold(0.05f);
	return rigid_dynamic;
}
