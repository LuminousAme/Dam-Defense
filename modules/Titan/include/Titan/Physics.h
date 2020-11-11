//Titan Engine, by Atlas X Games 
// Physics.h - header for the class that represents physics bodies
#pragma once

//include other headers
#include "Transform.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "Renderer.h"
//include glm features
#include "GLM/glm.hpp"
//import other required features
#include <vector>
//import the bullet physics engine
#include <bullet/btBulletDynamicsCommon.h>

namespace Titan {
	class TTN_Physics
	{
	public:

		//default constructor
		TTN_Physics();

		//aabb
		TTN_Physics(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, bool dynamic = true, float mass = 1.0f);

		~TTN_Physics();

		//copy, move, and assingment constrcutors for ENTT
		TTN_Physics(const TTN_Physics&) = default;
		TTN_Physics(TTN_Physics&&) = default;
		TTN_Physics& operator=(TTN_Physics&) = default;

		void Update(float deltaTime);

	protected:
		TTN_Transform m_trans; //transform with the position, rotation, and scale of the physics body

		bool m_IsStaticBody; //is the body dynamic or static

		//bullet data
		float m_Mass; //mass of the object
		btCollisionShape* m_colShape; //the shape of it's collider, includes scale
		btTransform m_bulletTrans;  //it's internal transform, does not include scale
		btDefaultMotionState* m_MotionState; //motion state for it, need to extract the transform out of this every update if the body is dynamic
		btRigidBody* m_body; //rigidbody, acutally does the collision stuff, have to get the transform out of this every update if the body is static
		bool m_InWorld; //boolean marking if it's been added to the bullet physics world yet, used to make sure that the physics body

		//data for rendering the physics box
		static TTN_Shader::sshptr shader;
		static TTN_Texture::stptr texture;
		static TTN_Material::smatptr mat;
		static TTN_Mesh::smptr mesh;
		static bool renderingSetUp;
		static TTN_Renderer renderer;
	};
}