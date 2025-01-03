
#include "Player.h"

#include "utils.h"


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
	_px_transform.p = physx::PxVec3(_position.x, _position.y + _aabb_hy, _position.z); // ignore rotation first
	rigid_dynamic->setKinematicTarget(_px_transform);
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
		limiting();
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
	_px_transform.p += physx::PxVec3(0.0f, _aabb_hy, 0.0f);
	rigid_dynamic = physics->createRigidDynamic(_px_transform);
	{
		//physx::PxShape* shape = physics->createShape(physx::PxTriangleMeshGeometry(px_triangle_mesh), *material);
		physx::PxShape* shape = physics->createShape(physx::PxBoxGeometry(0.5f, _aabb_hy, 0.5f), *material);
		rigid_dynamic->attachShape(*shape);
		shape->release();
	}
	rigid_dynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
	return rigid_dynamic;
}

void Player::updateSimulateResult() {
	// update PxTransform
	//physx::PxTransform px_new_transform = rigid_dynamic->getGlobalPose();
	//if (px_new_transform == _px_transform) return;

	//std::cout << "[Player][prev px_transform]" << std::endl;
	//printPxTransform(_px_transform);
	//std::cout << "[Player][new  px_transform]" << std::endl;
	//printPxTransform(px_new_transform);

	//_px_transform = px_new_transform;
	//// convert to glm positon and rotate
	//_position = glm::vec3(_px_transform.p.x, _px_transform.p.y - _aabb_hy, _px_transform.p.z);
	//_rotation = glm::quat(_px_transform.q.w, _px_transform.q.x, _px_transform.q.y, _px_transform.q.z);
	//// finally update model matrix and norm model matrix
	//_model_matrix = createModelMatrix(_position, _rotation, _scale);
	//updateNormModelMatrix();
}