
#include "Object.h"

#include <vector>

Object::Object(std::shared_ptr<Shader> shader, std::shared_ptr<Model> model, glm::mat4 model_matrix) {
	_type = MODEL;
	_model = model;
	_shader = shader;

	_model_matrix = model_matrix; // glm::mat4(1.0f);
	updateNormModelMatrix();
	// in details
	_position = glm::vec3(model_matrix[3][0], model_matrix[3][1], model_matrix[3][2]);
	_scale = 1.0f;
	glm::mat3 rotate_mat = glm::mat3(model_matrix);
	//rotate_mat = glm::normalize(rotate_mat);
	_rotation = glm::quat_cast(rotate_mat);
	
	// Set Animator
	animator = std::make_shared<Animator>(model->animation);
}


void Object::draw(std::shared_ptr<Shader>& shader) {
	/* Implement Model Matrix(&Normal Model Matrix) */
	shader->setMat4f("model", glm::value_ptr(_model_matrix));
	//shader->setMat4f("normModel", glm::value_ptr(_norm_model_matrix));

	/* Implement Animation Bone Matrices */
	const std::vector<glm::mat4>& bone_transforms = animator->getFinalBoneMatrices();
	for (int i = 0; i < bone_transforms.size(); ++i) {
		shader->setMat4f("finalBonesMatrices[" + std::to_string(i) + "]", glm::value_ptr(bone_transforms[i]));
	}

	_model->draw(shader.get());
}

void Object::update(float delta_time) {
	animator->update(delta_time);
}

void Object::render(glm::vec3& view, glm::vec3& projection) { // DO NOT USE
	_shader->begin();

	// TODO: whether to render here or out in the world
	_model->draw(_shader.get());

	_shader->end();
}

inline void Object::updateNormModelMatrix() {
	_norm_model_matrix = glm::transpose(glm::inverse(glm::mat3(_model_matrix)));
}


/* Physics */

// private
void Object::cookAndCreateTriangleMesh(physx::PxPhysics* physics, physx::PxCookingParams& cookingParams) {
	const auto& vertices = _model->px_combined_vertices;
	const auto& indices = _model->px_combined_indices;

	physx::PxTriangleMeshDesc mesh_desc;
	mesh_desc.points.count = vertices.size();
	mesh_desc.points.stride = sizeof(physx::PxVec3);
	mesh_desc.points.data = vertices.data();
	mesh_desc.triangles.count = indices.size() / 3;
	mesh_desc.triangles.stride = 3 * sizeof(physx::PxU32);
	mesh_desc.triangles.data = indices.data();

	physx::PxDefaultMemoryOutputStream write_buffer;
	if (!PxCookTriangleMesh(cookingParams, mesh_desc, write_buffer)) {
		std::cout << "Failed to cook convex mesh." << std::endl;
		return;
	}

	physx::PxDefaultMemoryInputData read_buffer(write_buffer.getData(), write_buffer.getSize());
	px_triangle_mesh = physics->createTriangleMesh(read_buffer);
}

// public
physx::PxRigidStatic* Object::createRigidStatic(physx::PxPhysics* physics, physx::PxCookingParams& cookingParams, physx::PxMaterial* material) {
	cookAndCreateTriangleMesh(physics, cookingParams);

	px_type = STATIC;
	_px_transform = physx::PxTransform(
		physx::PxVec3(_position.x, _position.y, _position.z),
		physx::PxQuat(_rotation.x, _rotation.y, _rotation.z, _rotation.w)
	);
	rigid_static = physics->createRigidStatic(_px_transform);
	{
		physx::PxShape* shape = physics->createShape(physx::PxTriangleMeshGeometry(px_triangle_mesh), *material);
		rigid_static->attachShape(*shape);
		shape->release();
	}
	return rigid_static;
}

physx::PxRigidDynamic* Object::createRigidDynamic(physx::PxPhysics* physics, physx::PxCookingParams& cookingParams, physx::PxMaterial* material) {
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
	return rigid_dynamic;
}

void Object::setRigidBodyFlag(physx::PxRigidBodyFlag::Enum flag, bool value) {
	if (px_type == DYNAMIC) {
		rigid_dynamic->setRigidBodyFlag(flag, value);
	}
}

void Object::updateSimulateResult() {
	if (px_type != DYNAMIC) return;
	
	std::cout << "[previous model_mat] " << glm::to_string(_model_matrix) << std::endl;

	// update PxTransform
	rigid_dynamic->getGlobalPose();
	// convert to glm positon and rotate
	_position = glm::vec3(_px_transform.p.x, _px_transform.p.y, _px_transform.p.z);
	_rotation = glm::quat(_px_transform.q.w, _px_transform.q.x, _px_transform.q.y, _px_transform.q.z);
	// finally update model matrix and norm model matrix
	_model_matrix = glm::translate(glm::mat4_cast(_rotation), _position);
	updateNormModelMatrix();

	std::cout << "[new model_mat] " << glm::to_string(_model_matrix) << std::endl;
}