#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <memory>
#include <random>

/* API & Window */
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/* Model */
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/* Physics */
#ifdef PHYSIC_IMPL
	#include <PhysX5/PxPhysicsAPI.h>
#endif

/* Sound */
#include <irrklang/irrKlang.h>

/* Math */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // for print
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

extern uint32_t viewport_width;
extern uint32_t viewport_height;
extern bool keyboard[128];
inline bool is_wasd() { return keyboard[GLFW_KEY_W] || keyboard[GLFW_KEY_A] || keyboard[GLFW_KEY_S] || keyboard[GLFW_KEY_D]; }

extern irrklang::ISoundEngine* sound_engine;

extern bool kinematicTouchStatic;

using ResizeCB = void(*)(int, int);
using KeyboardCB = void(*)(int, int, int, int);
using MouseMoveCB = void(*)(double, double);
using MouseScrollCB = void(*)(double);

enum Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

enum PXType {
	NONE = 0,	// have not allocated
	STATIC,		// PxRigidStatic
	DYNAMIC,	// PxRigidBody - PxRigidDynamic
	ARTLINK		// PxRigidBody - PxArticulationLink
};

//using namespace physx;

#endif // !COMMON_H
