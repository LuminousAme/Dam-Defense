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
#include "entt.hpp"
//import the bullet physics engine
#include <bullet/btBulletDynamicsCommon.h>

namespace Titan {
	class TTN_Physics
	{
	public:

		//default constructor
		TTN_Physics();

		//contrustctor with data
		TTN_Physics(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, entt::entity entityNum, bool dynamic = true, float mass = 1.0f);

		~TTN_Physics();

		//copy, move, and assingment constrcutors for ENTT
		TTN_Physics(const TTN_Physics&) = default;
		TTN_Physics(TTN_Physics&&) = default;
		TTN_Physics& operator=(TTN_Physics&) = default;

		//update function, keeps data up to date, call once a frame
		void Update(float deltaTime);

		//getters
		TTN_Transform GetTrans() { return m_trans; }
		bool GetIsStatic() { return m_IsStaticBody; }
		float GetMass() { return m_Mass; }
		btRigidBody* GetRigidBody() { return m_body; }
		bool GetIsInWorld() { return m_InWorld; }
		glm::vec3 GetLinearVelocity();
		glm::vec3 GetAngularVelocity();
		bool GetHasGravity() { return m_hasGravity; }
		entt::entity GetEntity() { return m_entity; }

		//setters
		void SetIsStatic(bool isStatic);
		void SetIsInWorld(bool inWorld);
		void SetMass(float mass);
		void SetLinearVelocity(glm::vec3 velocity);
		void SetAngularVelocity(glm::vec3 velocity);
		void SetHasGravity(bool hasGrav);

		//forces
		void AddForce(glm::vec3 force);
		void AddImpulse(glm::vec3 impulseForce);
		void ClearForces();

		//identifier
		void SetEntity(entt::entity entity);

	protected:
		TTN_Transform m_trans; //transform with the position, rotation, and scale of the physics body

		bool m_IsStaticBody; //is the body dynamic or static

		//bullet data
		float m_Mass; //mass of the object
		bool m_hasGravity; //is the object affected by gravity
		btCollisionShape* m_colShape; //the shape of it's collider, includes scale
		btTransform m_bulletTrans;  //it's internal transform, does not include scale
		btDefaultMotionState* m_MotionState; //motion state for it, need to extract the transform out of this every update if the body is dynamic
		btRigidBody* m_body; //rigidbody, acutally does the collision stuff, have to get the transform out of this every update if the body is static
		bool m_InWorld; //boolean marking if it's been added to the bullet physics world yet, used to make sure that the physics body

		entt::entity m_entity; //the entity number that gets stored as a void pointer in bullet so that it can be used to indentify the objects later

		//data for rendering the physics box
		static TTN_Shader::sshptr shader;
		static TTN_Texture2D::st2dptr texture;
		static TTN_Material::smatptr mat;
		static TTN_Mesh::smptr mesh;
		static bool renderingSetUp;
		static TTN_Renderer renderer;
	};

	class TTN_Collision {
	public:
		//defines a special easier to use name for the shared(smart) pointer to the class
		typedef std::shared_ptr<TTN_Collision> scolptr;

		//creates and returns a shared(smart) pointer to the class
		static inline scolptr Create() {
			return std::make_shared<TTN_Collision>();
		}
	public:
		//ensure moving and copying is not allowed so we can control destructor calls through pointers
		TTN_Collision(const TTN_Collision& other) = delete;
		TTN_Collision(TTN_Collision& other) = delete;
		TTN_Collision& operator=(const TTN_Collision& other) = delete;
		TTN_Collision& operator=(TTN_Collision&& other) = delete;

	public:
		//constructor
		TTN_Collision();

		//destructor
		~TTN_Collision() = default;

		//getters
		const btRigidBody* GetBody1() { return b1; }
		const btRigidBody* GetBody2() { return b2; }
		glm::vec3 GetNormal1() { return norm1; }
		glm::vec3 GetNormal2() { return norm2; }

		//setters
		void SetBody1(const btRigidBody* body);
		void SetBody2(const btRigidBody* body);
		void SetNormal1(btVector3 normal);
		void SetNormal2(btVector3 normal);

	protected:
		//rigidbodies for the colliding objects (which should also contain a reference to the entity)
		const btRigidBody* b1;
		const btRigidBody* b2;
		//normals for the collision
		glm::vec3 norm1;
		glm::vec3 norm2;
	};
}