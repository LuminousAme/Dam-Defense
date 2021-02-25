//Dam Defense, by Atlas X Games
//GameOverMenu.h, the header file for the class that represents the game over menu
#pragma once

//include required features from titan
#include "Titan/Application.h"
#include "Titan/Utilities/Interpolation.h"

using namespace Titan;

class GameOverMenu : public TTN_Scene {
public:
	//default constructor
	GameOverMenu();

	~GameOverMenu()=default;

	//sets up the scene
	void InitScene();

	//update the scene
	void Update(float deltaTime);

	//late render (terrain, water, etc.)
	void PostRender();

	//Assets
public:

	//shader programs
	TTN_Shader::sshptr shaderProgramTextured;
	TTN_Shader::sshptr shaderProgramSkybox;
	TTN_Shader::sshptr shaderProgramAnimatedTextured;
	TTN_Shader::sshptr shaderProgramWater;
	TTN_Shader::sshptr shaderProgramTerrain;

	//meshes
	TTN_Mesh::smptr cannonMesh;
	TTN_Mesh::smptr flamethrowerMesh;
	TTN_Mesh::smptr damMesh;
	TTN_Mesh::smptr skyboxMesh;
	TTN_Mesh::smptr sphereMesh; //used for cannonballs and particles
	TTN_Mesh::smptr terrainPlain;

	//textures
	TTN_Texture2D::st2dptr cannonText;
	TTN_TextureCubeMap::stcmptr skyboxText;
	TTN_Texture2D::st2dptr terrainMap;
	TTN_Texture2D::st2dptr sandText;
	TTN_Texture2D::st2dptr rockText;
	TTN_Texture2D::st2dptr grassText;
	TTN_Texture2D::st2dptr waterText;
	TTN_Texture2D::st2dptr flamethrowerText;
	TTN_Texture2D::st2dptr damText;

	//materials
	TTN_Material::smatptr flamethrowerMat;
	TTN_Material::smatptr damMat;
	TTN_Material::smatptr cannonMat;
	TTN_Material::smatptr skyboxMat;

	//Entities
protected:
	entt::entity camera;
	entt::entity light;
	entt::entity skybox;
	entt::entity cannon;
	entt::entity terrain;
	entt::entity water;
	entt::entity dam;

	std::vector<entt::entity> flamethrowers;

	//other data
protected:
	//the terrain scale
	float terrainScale;
	//water animation control
	float time;
	float waveSpeed;
	float waveBaseHeightIncrease;
	float waveHeightMultiplier;
	float waveLenghtMultiplier;

	//Stuff for waves and spawning enemies
	float Timer = 0.F;//timer for boat spawning (left side)
	float Timer2 = 0.F;//timer for boat spawning (right side)
	bool Spawning = true; //whether or not the spawners should be spawning

	float waveTimer = 0.F;//timer for waves
	float restTimer = 0.F;//timer for waves
	int wave = 0; // keep track of wave number

	//set up functions, called by InitScene()
protected:
	void SetUpAssets();
	void SetUpEntities();
	void SetUpOtherData();

	//CG assingment 2 stuff
protected:
	//color correction effect
	TTN_ColorCorrect::scolcorptr m_colorCorrectEffect;
	//bools for imgui controls
	bool m_applyWarmLut;
	bool m_applyCoolLut;
	bool m_applyCustomLut;

	bool m_noLighting;
	bool m_ambientOnly;
	bool m_specularOnly;
	bool m_ambientAndSpecular;
	bool m_ambientSpecularAndOutline;
	//float to control outline size
	float m_outlineSize = 0.2f;

	std::vector<TTN_Material::smatptr> m_mats;
	//variables for if the specular and diffuse ramps should be used
	bool m_useDiffuseRamp = false;
	bool m_useSpecularRamp = false;
	bool m_useTextures = true;

	void ImGui();
};


class GameOverMenuUI : public TTN_Scene {
public:
	//default constructor
	GameOverMenuUI();

	//default destrcutor 
	~GameOverMenuUI() = default;

	//sets up the scene
	void InitScene();

	//update the scene
	void Update(float deltaTime);

	//mouse input
	void MouseButtonDownChecks();

	void SetShouldPlay(bool play) { shouldPlay = play; }
	void SetShouldQuit(bool quit) { shouldQuit = quit; }
	void SetShouldMenu(bool menu) { shouldMenu = menu; }

	bool GetShouldPlay() { return shouldPlay; }
	bool GetShouldQuit() { return shouldQuit; }
	bool GetShouldMenu() { return shouldMenu; }

private:
	//entities
	entt::entity cam;
	entt::entity gameOver;
	//play
	entt::entity playButton;
	entt::entity playText;
	//main menu
	entt::entity menuButton;
	entt::entity menuText;
	//quit
	entt::entity quitButton;
	entt::entity quitText;

	//assets
	TTN_Texture2D::st2dptr textureGameLogo;
	TTN_Texture2D::st2dptr textureButton1;
	TTN_Texture2D::st2dptr textureButton2;
	TTN_Texture2D::st2dptr texturePlayAgain;
	TTN_Texture2D::st2dptr textureScore;
	TTN_Texture2D::st2dptr textureMainMenu;
	TTN_Texture2D::st2dptr textureGameOver;
	TTN_Texture2D::st2dptr textureQuit;

	bool shouldQuit;
	bool shouldPlay;
	bool shouldMenu;
};