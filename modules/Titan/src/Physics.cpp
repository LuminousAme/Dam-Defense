//Titan Engine, by Atlas X Games 
// Physics.cpp - source file for the class that represents physics bodies

//include the header
#include "Titan/Physics.h"
//include other required features
#include "Titan/ObjLoader.h"
#include <iostream>
#include "..\include\Titan\Physics.h"

namespace Titan {

	//initlization for static rendering variables
	TTN_Shader::sshptr TTN_Physics::shader = nullptr;
	TTN_Texture2D::stptr TTN_Physics::texture = nullptr;
	TTN_Material::smatptr TTN_Physics::mat = nullptr;
	TTN_Mesh::smptr TTN_Physics::mesh = nullptr;
	bool TTN_Physics::renderingSetUp = false;
	TTN_Renderer TTN_Physics::renderer = TTN_Renderer();

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
		m_IsStaticBody = false;
		if (m_IsStaticBody) {
			m_Mass = 0.0f;
			m_colShape->calculateLocalInertia(m_Mass, localIntertia);
		}

		//create the rigidbody
		btRigidBody::btRigidBodyConstructionInfo rbInfo(m_Mass, m_MotionState, m_colShape, localIntertia);
		m_body = new btRigidBody(rbInfo);

		m_hasGravity = true;

		m_InWorld = false;

		m_entity = static_cast<entt::entity>(-1);


		m_body->setUserPointer(&m_entity);
	
	}

	//constructor that makes a physics body out of a position, rotation, and scale
	Titan::TTN_Physics::TTN_Physics(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, entt::entity entityNum, bool dynamic, float mass)
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
		m_IsStaticBody = (!dynamic);
		if (m_IsStaticBody) {
			m_Mass = 0.0f;
			m_colShape->calculateLocalInertia(m_Mass, localIntertia);
		}

		//create the rigidbody
		btRigidBody::btRigidBodyConstructionInfo rbInfo(m_Mass, m_MotionState, m_colShape, localIntertia);
		m_body = new btRigidBody(rbInfo);

		m_hasGravity = true;

		m_InWorld = false;

		m_entity = entityNum;

		m_body->setUserPointer(static_cast<void*>(&m_entity));
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

	void TTN_Physics::SetIsStatic(bool isStatic)
	{
		m_IsStaticBody = isStatic;
		if (m_IsStaticBody) {
			SetMass(0.0f);
		}
		else if (!m_IsStaticBody && m_Mass == 0.0f) {
			SetMass(1.0f);
		}
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
		if (m_Mass == 0.0f) {
			//if it isn't mark it as static
			m_IsStaticBody = true;
		}
		else {
			//if it is, mark it as dynamic, and restore the velocities
			m_IsStaticBody = false;
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
		m_body->setUserPointer(static_cast<void*>(&m_entity));
	}

	TTN_Collision::TTN_Collision()
	{
		b1 = nullptr;
		b2 = nullptr;
		norm1 = glm::vec3();
		norm2 = glm::vec3();
	}

	void TTN_Collision::SetBody1(const btRigidBody* body)
	{
		b1 = body;
	}

	void TTN_Collision::SetBody2(const btRigidBody* body)
	{
		b2 = body;
	}

	void TTN_Collision::SetNormal1(btVector3 normal)
	{
		norm1 = glm::vec3((float)normal.getX(), (float)normal.getY(), (float)normal.getZ());
	}

	void TTN_Collision::SetNormal2(btVector3 normal)
	{
		norm2 = glm::vec3((float)normal.getX(), (float)normal.getY(), (float)normal.getZ());
	}
}