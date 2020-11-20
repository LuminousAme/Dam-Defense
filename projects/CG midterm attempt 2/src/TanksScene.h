//INFR 1350 Pratical Midterm - Battle Tanks by Ame Gilham (100741352) and Jackie Zhou (100750922)
//The project makes use of the Titan Engine, the engine we developed together through this class and GDW (team: Atlas X Games)
//all classes with the prefix TTN_ are classes from the Titan engine

//TanksScene.h, the header for the class for the scene with the game battle tanks
#pragma once

//include titan features
#include "Titan/Application.h"
#include "Titan/ObjLoader.h"
//include other features
#include <ctime>
#include "glm/ext.hpp"
#include <GLM/gtx/rotate_vector.hpp>

using namespace Titan;

//child class of the scene class to be the scene to play the game battle tanks
class TanksScene : public TTN_Scene {
public:
	//default constructor
	TanksScene();
	//default destructor
	~TanksScene() = default;

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
	//shader programs
	TTN_Shader::sshptr shaderProgram;

	//meshes
	TTN_Mesh::smptr tankMesh;
	TTN_Mesh::smptr cannonballMesh;
	TTN_Mesh::smptr planeMesh;
	TTN_Mesh::smptr cubeMesh;
	TTN_Mesh::smptr expolsionMesh;
	TTN_Mesh::smptr scoreMesh[10];

	//textures
	TTN_Texture2D::st2dptr redTankText;
	TTN_Texture2D::st2dptr blueTankText;
	TTN_Texture2D::st2dptr backgroundText;
	TTN_Texture2D::st2dptr wallText;
	TTN_Texture2D::st2dptr redScoreText;
	TTN_Texture2D::st2dptr blueScoreText;
	TTN_Texture2D::st2dptr expolsionText;

	//materials
	TTN_Material::smatptr redTankMat;
	TTN_Material::smatptr blueTankMat;
	TTN_Material::smatptr backgroundMat;
	TTN_Material::smatptr wallMat;
	TTN_Material::smatptr redScoreMat;
	TTN_Material::smatptr blueScoreMat;
	TTN_Material::smatptr expolsionMat;

protected:
	//sets up the resources (shaders, meshes, textures, mats, etc.)
	void InitResources();

	//sets up the entities that serve as actual game objects
	void InitEntities();

	//sets up other data the scene needs
	void InitOtherData();

protected:
	//entities
	entt::entity camera;
	entt::entity mainLight;
	entt::entity backgroundPlane;
	entt::entity redPlayer;
	entt::entity lightRedPlayer;
	entt::entity bluePlayer;
	entt::entity lightBluePlayer;
	entt::entity walls[44];
	entt::entity redScoreEntity;
	entt::entity blueScoreEntity;
	entt::entity redBullet;
	entt::entity blueBullet;
	entt::entity redExpolsion;
	entt::entity blueExpolsion;

	//others
	//speed of the players 
	float playerSpeed;

	//the directions the players are facing
	glm::vec2 redPlayerDir;
	glm::vec2 bluePlayerDir;

	//the score of each player
	int scoreRed;
	int scoreBlue;

	//controls for the canoonballs of both tanks firing
	float RedBufferUntilNextBounce; //control to prevent the cannonballs' bouncing to registry several times for a single bounce
	float BlueBufferUntilNextBounce;
	int bounceCountR;//tracks the number of times each cannonball has bounced
	int bounceCountB;

	//screenshake control
	//original camera pos
	glm::vec3 originalCamPos;
	float shakeDuration;
	float shakeMagnitude;

	//expolsion control
	glm::vec3 originalExpolsionPos;
	float redExpolsionDuration;
	float blueExpolsionDuration;

	//create bullet
	void CreateRedBullet();
	void CreateBlueBullet();

	bool redBulletExists;
	bool blueBulletExists;

	//create players 
	void CreateRedPlayer();
	void CreateBluePlayer();
};