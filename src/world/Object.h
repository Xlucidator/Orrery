#ifndef OBJECT_H
#define OBJECT_H

#include "common.h"
#include "shader/Shader.h"
#include "model/Model.h"
#include "animation/Animator.h"

#include <string>

enum OBJType {
	MODEL = 0,
	SIMPLE
};

class Object {
public:
	Object(std::shared_ptr<Shader> shader, std::shared_ptr<Model> model, glm::mat4 model_matrix = glm::mat4(1.0f));
	~Object() {
		if (rigid_static) { rigid_static->release(); }
		if (rigid_dynamic) { rigid_dynamic->release();  }
		if (px_triangle_mesh) { px_triangle_mesh->release(); }
	}

	void render(glm::vec3& view, glm::vec3& projection); // TODO: whether render here or out
	void draw(std::shared_ptr<Shader>& shader);
	//void update();

	void setModelMatrix(glm::mat4 model) { 
		_model_matrix = model;
		updateNormModelMatrix();
	}
	glm::mat4 getModelMatrix() { return _model_matrix; }
	glm::mat3 getNormModelMatrix() { return _norm_model_matrix; }

	/* Physics */
	// PxActor
	//	©¸©¤©¤ PxRigidActor
	//		©À©¤©¤ PxRigidStatic
	//		©¸©¤©¤ PxRigidBody
	//				©À©¤©¤ PxRigidDynamic
	//				©¸©¤©¤ PxArticulationLink
	// physx::PxActorType::Enum px_atype = physx::PxActorType::Enum::eINVALID;
	// physx::PxActor* rigid_actor = nullptr;
	PXType px_type = NONE;
	physx::PxRigidStatic* rigid_static = nullptr;
	physx::PxRigidDynamic* rigid_dynamic = nullptr;
	physx::PxRigidStatic* createRigidStatic(physx::PxPhysics* physics, physx::PxCookingParams& cookingParams, physx::PxMaterial* material);
	physx::PxRigidDynamic* createRigidDynamic(physx::PxPhysics* physics, physx::PxCookingParams& cookingParams, physx::PxMaterial* material);
	void setRigidBodyFlag(physx::PxRigidBodyFlag::Enum flag, bool value); // TODO: more clear
	void updateSimulateResult();

	/* Animator: Drive Animation */
	bool has_animator = false;
	std::shared_ptr<Animator> animator = nullptr;

private:
	OBJType _type;
	std::shared_ptr<Shader> _shader = nullptr; // shared
	std::shared_ptr<Model> _model = nullptr;   // shared
	
	glm::mat4 _model_matrix;
	glm::mat3 _norm_model_matrix;
	// in details
	glm::vec3 _position;
	float _scale; // TODO: do not use
	glm::quat _rotation;
	// for Px
	physx::PxTransform _px_transform;
	
	inline void updateNormModelMatrix();

	/* Physics */
	physx::PxTriangleMesh* px_triangle_mesh = nullptr;
	void cookAndCreateTriangleMesh(physx::PxPhysics* physics, physx::PxCookingParams& cookingParams);
};


#endif // !OBJECT_H
