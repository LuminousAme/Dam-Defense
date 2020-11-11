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
	TTN_Texture::stptr TTN_Physics::texture = nullptr;
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

		m_InWorld = false;
	}

	//constructor that makes a physics body out of a position, rotation, and scale
	Titan::TTN_Physics::TTN_Physics(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, bool dynamic, float mass)
	{
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

		m_InWorld = false;
	}

	TTN_Physics::~TTN_Physics()
	{
		delete m_body;
		delete m_MotionState;
		delete m_colShape;
	}


	//updates the position of the physics body based on the velocity and deltaTime
	void TTN_Physics::Update(float deltaTime)
	{
		//updates the titan transform of the physics body

		//fetch the bullet transform
		if (m_body->getMotionState != nullptr) {
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
}