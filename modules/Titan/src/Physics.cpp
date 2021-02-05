//Titan Engine, by Atlas X Games 
// Physics.cpp - source file for the class that represents physics bodies

//precompile header, this file uses iostream and stdio.h
#include "Titan/ttn_pch.h"
//include the header
#include "Titan/Physics.h"
//include other required features
#include "Titan/ObjLoader.h"

namespace Titan {
	//default constructor, constructs a basic 1x1x1 physics body around the origin
	TTN_Physics::TTN_Physics()
	{
		//set up titan transform
		m_trans = TTN_Transform();
		m_trans.SetPos(glm::vec3(0.0f));
		m_trans.SetScale(glm::vec3(1.0f));

		//set up bullet collision shape
		m_colShape = new btBoxShape(btVector3(m_trans.GetScale().x / 2.0f, m_trans.GetScale().y / 2.0f, m_trans.GetScale().z / 2.0f));
		//set up bullet transform
		m_bulletTrans.setIdentity();
		m_bulletTrans.setOrigin(btVector3(m_trans.GetPos().x, m_trans.GetPos().y, m_trans.GetPos().z));
		m_bulletTrans.setRotation(btQuaternion(m_trans.GetRotQuat().x, m_trans.GetRotQuat().y, m_trans.GetRotQuat().z, m_trans.GetRotQuat().w));
		//setup up bullet motion state
		m_MotionState = new btDefaultMotionState(m_bulletTrans);

		//setup mass, static v dynmaic status, and local internia
		btVector3 localIntertia(0, 0, 0);
		m_Mass = 1.0f;

		m_bodyType = TTN_PhysicsBodyType::DYNAMIC;

		//create the rigidbody
		btRigidBody::btRigidBodyConstructionInfo rbInfo(m_Mass, m_MotionState, m_colShape, localIntertia);
		m_body = new btRigidBody(rbInfo);

		m_body->setActivationState(DISABLE_DEACTIVATION);

		m_hasGravity = true;

		m_InWorld = false;

		m_entity = static_cast<entt::entity>(-1);

		m_body->setUserPointer(reinterpret_cast<void*>(static_cast<uint32_t>(m_entity)));
	
	}

	//constructor that makes a physics body out of a position, rotation, and scale
	TTN_Physics::TTN_Physics(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, entt::entity entityNum, TTN_PhysicsBodyType bodyType, float mass)
	{
		//set up titan transform
		m_trans = TTN_Transform();
		m_trans.SetPos(position);
		m_trans.RotateFixed(rotation);
		m_trans.SetScale(scale);

		//set up bullet collision shape
		m_colShape = new btBoxShape(btVector3(m_trans.GetScale().x / 2.0f, m_trans.GetScale().y / 2.0f, m_trans.GetScale().z / 2.0f));
		m_bulletTrans.setIdentity();
		//set up bullet transform
		m_bulletTrans.setOrigin(btVector3(m_trans.GetPos().x, m_trans.GetPos().y, m_trans.GetPos().z));
		m_bulletTrans.setRotation(btQuaternion(m_trans.GetRotQuat().x, m_trans.GetRotQuat().y, m_trans.GetRotQuat().z, m_trans.GetRotQuat().w));
		//setup up bullet motion state
		m_MotionState = new btDefaultMotionState(m_bulletTrans);

		//setup mass, static v dynmaic status, and local internia
		btVector3 localIntertia(0, 0, 0);
		m_Mass = mass;
		
		//take the body type 
		m_bodyType = bodyType;

		//if it's static or kinematic
		if (m_bodyType == TTN_PhysicsBodyType::STATIC || m_bodyType == TTN_PhysicsBodyType::KINEMATIC)
			m_Mass = 0;

		//create the rigidbody
		btRigidBody::btRigidBodyConstructionInfo rbInfo(m_Mass, m_MotionState, m_colShape, localIntertia);
		m_body = new btRigidBody(rbInfo);

		//if it's kinematic, set the kinematic flag
		if (m_bodyType == TTN_PhysicsBodyType::KINEMATIC) {
			m_body->setCollisionFlags(m_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		}
		else if (m_bodyType == TTN_PhysicsBodyType::STATIC) {
			m_body->setCollisionFlags(m_body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		}

		m_body->setActivationState(DISABLE_DEACTIVATION);

		m_hasGravity = true;

		m_InWorld = false;

		m_entity = entityNum;

		m_body->setUserPointer(reinterpret_cast<void*>(static_cast<uint32_t>(m_entity)));
	}

	TTN_Physics::~TTN_Physics()
	{}

	//updates the position of the physics body based on the velocity and deltaTime
	void TTN_Physics::Update(float deltaTime)
	{
		//updates the titan transform of the physics body

		//fetch the bullet transform
		if (m_body->getMotionState() != nullptr) {
			m_body->getMotionState()->getWorldTransform(m_bulletTrans);
		}
		else {
			m_bulletTrans = m_body->getWorldTransform();
		}

		//copy the position of the bullet transfrom into the titan transform
		m_trans.SetPos(glm::vec3((float)m_bulletTrans.getOrigin().getX(), (float)m_bulletTrans.getOrigin().getY(), (float)m_bulletTrans.getOrigin().getZ()));

		//copy the rotation
		btQuaternion rot = m_bulletTrans.getRotation();
		m_trans.SetRotationQuat(glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ()));
	}

	
	//reads the velocity out from bullet
	glm::vec3 TTN_Physics::GetLinearVelocity()
	{
		//get the linear velocity
		btVector3 velo = m_body->getLinearVelocity();
		//cast it to a glm vec3 and return it
		return glm::vec3((float)velo.getX(), (float)velo.getY(), (float)velo.getZ());
	}

	//gets the angular velocity out of bullet
	glm::vec3 TTN_Physics::GetAngularVelocity()
	{
		btVector3 velo = m_body->getAngularVelocity();
		return glm::vec3((float)velo.getX(), (float)velo.getY(), (float)velo.getZ());
	}

	glm::vec3 TTN_Physics::GetPos()
	{
		btTransform trans;
		m_body->getMotionState()->getWorldTransform(trans);
		btVector3 position = trans.getOrigin();
		return glm::vec3((float)position.getX(), (float)position.getY(), (float)position.getZ());
	}

	//sets the flag for if it's in the physics world or not
	void TTN_Physics::SetIsInWorld(bool inWorld)
	{
		m_InWorld = inWorld;
	}

	//sets the mass of the object
	void TTN_Physics::SetMass(float mass)
	{
		//save the mass
		m_Mass = mass;
		//update it in bullet
		//clear velocities
		btVector3 linearVelo = m_body->getLinearVelocity();
		btVector3 angularVelo = m_body->getAngularVelocity();
		m_body->setLinearVelocity(btVector3(0,0,0));
		m_body->setAngularVelocity(btVector3(0, 0, 0));
		//update mass
		m_body->setMassProps(m_Mass, btVector3(0,0,0));
		//check if the body is still dynamic
		if (m_Mass != 0.0f) {
			m_body->setLinearVelocity(linearVelo);
			m_body->setAngularVelocity(angularVelo);
		}
	}

	void TTN_Physics::SetLinearVelocity(glm::vec3 velocity)
	{
		m_body->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
	}

	void TTN_Physics::SetAngularVelocity(glm::vec3 velocity)
	{
		m_body->setAngularVelocity(btVector3(velocity.x, velocity.y, velocity.z));
	}

	void TTN_Physics::SetPos(glm::vec3 position)
	{
		btVector3 newPos = btVector3(position.x, position.y, position.z);
		btTransform Trans;
		m_body->getMotionState()->getWorldTransform(Trans);
		Trans.setOrigin(newPos);
		m_body->getMotionState()->setWorldTransform(Trans);
		m_trans.SetPos(position);
	}

	void TTN_Physics::SetHasGravity(bool hasGrav)
	{
		m_hasGravity = hasGrav;
	}

	void TTN_Physics::AddForce(glm::vec3 force)
	{
		m_body->applyCentralForce(btVector3(force.x, force.y, force.z));
	}

	void TTN_Physics::AddImpulse(glm::vec3 impulseForce)
	{
		m_body->applyCentralImpulse(btVector3(impulseForce.x, impulseForce.y, impulseForce.z));
	}

	void TTN_Physics::ClearForces()
	{
		m_body->clearForces();
	}
	void TTN_Physics::SetEntity(entt::entity entity)
	{
		//save the entity in titan
		m_entity = entity;
		//save the entity in bullet
		m_body->setUserPointer(reinterpret_cast<void*>(static_cast<uint32_t>(m_entity)));
	}

	TTN_Collision::TTN_Collision()
	{
		b1 = entt::null;
		b2 = entt::null;
	}

	void TTN_Collision::SetBody1(const entt::entity body)
	{
		b1 = body;
	}

	void TTN_Collision::SetBody2(const entt::entity body)
	{
		b2 = body;
	}

	void TTN_Collision::SetCollisionPoint(const glm::vec3 point)
	{
		collisionPoint = point;
	}
}