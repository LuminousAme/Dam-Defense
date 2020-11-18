//Dam Defense by Atlas X Games 
//Review3Scene.h - header for the class that inherits from the scene class to make a scene represneting a demo of our final product for review 3
#pragma once

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

	//meshes
	TTN_Mesh::smptr cannonMesh;
	TTN_Mesh::smptr plane;
	TTN_Mesh::smptr tree1Mesh;

	//textures
	TTN_Texture2D::st2dptr waterText;
	TTN_Texture2D::st2dptr cannonText;

	//materials
	TTN_Material::smatptr waterMat;
	TTN_Material::smatptr cannonMat;

protected:
	//entities
	entt::entity camera;
	entt::entity light;
	entt::entity cannon;
	entt::entity water;
	entt::entity tree1;
	std::vector<entt::entity> cannonballs;


	//others
	glm::vec2 rotAmmount;
	glm::vec2 mousePos; 
};