//Dam Defense by Atlas X Games 
//DemoScene.h - header for the class that inherits from the scene class to make a useable scene
#pragma once

#include "Titan/Application.h"
#include "Titan/Physics.h"
#include "Titan/Particle.h"
#include "Titan/ObjLoader.h"
#include <iostream>

using namespace Titan;

class DemoScene: public TTN_Scene {
public:
	DemoScene();

	//sets up the scene
	void InitScene();

	//update the scene
	void Update(float deltaTime);

	//keyboard input
	void KeyDownChecks();
	void KeyChecks();
	void KeyUpChecks();

	//mouse input
	void MouseButtonDownChecks();
	void MouseButtonChecks();
	virtual void MouseButtonUpChecks();

public:
	TTN_Shader::sshptr shaderProgam;
	TTN_Shader::sshptr shaderProgamTextured;

	TTN_Mesh::smptr boatMesh;
	TTN_Mesh::smptr treeMesh;
	TTN_Mesh::smptr swordMesh;
	TTN_Mesh::smptr plane;

	TTN_Texture2D::st2dptr swordText;
	TTN_Texture2D::st2dptr waterText;

	TTN_Material::smatptr swordMat;
	TTN_Material::smatptr waterMat;

	TTN_Particle ParticleSystem;
	ParticleData Particles;

protected:
	entt::entity camera;
	entt::entity light;
	entt::entity sword;
	entt::entity tree1;
	entt::entity tree2;
	entt::entity boat;
	entt::entity water;

	float speed;
	glm::vec3 velo;
};