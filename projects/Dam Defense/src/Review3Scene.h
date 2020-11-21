//Dam Defense by Atlas X Games 
//Review3Scene.h - header for the class that inherits from the scene class to make a scene represneting a demo of our final product for review 3
#pragma once

//#include "Titan/Particle.h"
#include "Titan/Application.h"
#include "Titan/ObjLoader.h"
#include <iostream>

using namespace Titan;

class Review3Scene : public TTN_Scene {
public:
	//default constructor
	Review3Scene();

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

	void CreateCannonball();

public:
	//shader programs
	TTN_Shader::sshptr shaderProgam;
	TTN_Shader::sshptr shaderProgamTextured;
	TTN_Shader::sshptr shaderHeight;
	TTN_Shader::sshptr shaderProgramSkybox;

	//meshes
	TTN_Mesh::smptr cannonMesh;
	TTN_Mesh::smptr plane;
	TTN_Mesh::smptr tree1Mesh;
	TTN_Mesh::smptr skyboxMesh;

	//textures
	TTN_Texture2D::st2dptr waterText;
	TTN_Texture2D::st2dptr cannonText;
	TTN_TextureCubeMap::stcmptr skyboxText;
	TTN_Texture2D::st2dptr heightmap;

	//materials
	TTN_Material::smatptr waterMat;
	TTN_Material::smatptr cannonMat;
	TTN_Material::smatptr skyboxMat;
	TTN_Material::smatptr heightMat;

	//TTN_Particle ParticleSystem;
	//ParticleData Particles;

protected:
	//entities
	entt::entity camera;
	entt::entity light;
	entt::entity cannon;
	entt::entity water;
	entt::entity tree1;
	entt::entity skybox;
	entt::entity height;
	std::vector<entt::entity> cannonballs;


	//others
	glm::vec2 rotAmmount;
	glm::vec2 mousePos; 
};