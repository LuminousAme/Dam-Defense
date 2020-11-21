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
	enum class TTN_PhysicsBodyType {
		STATIC = 0,
		DYNAMIC = 1,
		KINEMATIC = 2
	};

	class TTN_Physics
	{
	public:
		//default constructor
		TTN_Physics();

		//contrustctor with data
		TTN_Physics(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, entt::entity entityNum, TTN_PhysicsBodyType bodyType = TTN_PhysicsBodyType::DYNAMIC, float mass = 1.0f);

		~TTN_Physics();

		//copy, move, and assingment constrcutors for ENTT
		TTN_Physics(const TTN_Physics&) = default;
		TTN_Physics(TTN_Physics&&) = default;
		TTN_Physics& operator=(TTN_Physics&) = default;

		//update function, keeps data up to date, call once a frame
		void Update(float deltaTime);

		//getters
		TTN_Transform GetTrans() { return m_trans; }
		bool GetIsStatic() {
			if (m_bodyType == TTN_PhysicsBodyType::STATIC) return true;
			else return false;
		}
		bool GetIsDynamic() {
			if (m_bodyType == TTN_PhysicsBodyType::DYNAMIC) return true;
			else return false;
		}
		bool GetIsKinematic() {
			if (m_bodyType == TTN_PhysicsBodyType::KINEMATIC) return true;
			else return false;
		}
		float GetMass() { return m_Mass; }
		btRigidBody* GetRigidBody() { return m_body; }
		bool GetIsInWorld() { return m_InWorld; }
		glm::vec3 GetLinearVelocity();
		glm::vec3 GetAngularVelocity();
		bool GetHasGravity() { return m_hasGravity; }
		entt::entity GetEntity() { return m_entity; }

		//setters
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

		TTN_PhysicsBodyType m_bodyType;

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
		const entt::entity GetBody1() { return b1; }
		const entt::entity GetBody2() { return b2; }

		//setters
		void SetBody1(const entt::entity body);
		void SetBody2(const entt::entity body);

		//checks if two collisions pointers represent a collision between the same objects
		static bool same(scolptr collision1, scolptr collision2) {
			//compare the entity numbers
			if ((collision1->b1 == collision2->b1 && collision1->b2 == collision2->b2) ||
				(collision1->b1 == collision2->b2 && collision1->b2 == collision2->b1)) {
				//if they match across the collisions, then return true
				return true;
			}
			else
				//otherwise return false
				return false;
		}

	protected:
		//rigidbodies for the colliding objects (which should also contain a reference to the entity)
		entt::entity b1;
		entt::entity b2;
	};
}