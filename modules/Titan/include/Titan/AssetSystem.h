//Titan Engine, by Atlas X Games 
// AssetSystem.h - header for the class that manages assets, allowing them to be loaded and accessed dynamically
#pragma once

//include the precompile header with a bunch of system
#include "ttn_pch.h"
//include all of the various asset classes and their loaders
#include "Texture2D.h"
#include "TextureCubeMap.h"
#include "Mesh.h"
#include "ObjLoader.h"
#include "Shader.h"
#include "Material.h"

namespace Titan {
	//class to control all the assets in any given project
	class TTN_AssetSystem {
	public:
		//default destructor
		~TTN_AssetSystem() = default;

		/////////////functions for loading assets/////////////////////
		//Adds a 2D texture to the list of assets to be loaded
		static void AddTexture2DToBeLoaded(std::string fileName, int set = 0);
		//Adds a Skybox to the list of assets to be loaded
		static void AddSkyboxToBeLoaded(std::string fileName, int set = 0);
		//Adds a mesh to the list of assets to be loaded
		static void AddMeshToBeLoaded(std::string fileName, int set = 0);
		//Adds a morph animation mesh to the list of assets to be loaded
		static void AddMorphAnimationMeshesToBeLoaded(std::string fileName, int NumOfFrames, int set = 0);
		//Adds a shader to the list of assets to be loaded
		static void AddShaderToBeLoaded(std::string accessName, std::string VertShaderFileName, std::string FragShaderFileName, int set = 0);
		//Adds a Titan default shader to be loaded
		static void AddDefaultShaderToBeLoaded(std::string accessName, TTN_DefaultShaders VertShader, TTN_DefaultShaders FragShader, int set = 0);
		//Creates a new material pointer in the system 
		static void CreateNewMaterial(std::string accessName);

		/////////////functions for adding preexisting/loaded assets into the system/////////////////////
		//Adds an already loaded 2D texture to the system
		static void AddExisting2DTexture(std::string accessName, TTN_Texture2D::st2dptr texture);
		//Adds an already loaded skybox
		static void AddExistingSkybox(std::string accessName, TTN_TextureCubeMap::stcmptr cubeMap);
		//Adds an already loaded mesh
		static void AddExistingMesh(std::string accessName, TTN_Mesh::smptr mesh);
		//Adds an already loaded Shader Program
		static void AddExistingShader(std::string accessName, TTN_Shader::sshptr shader);
		//Adds an already existing material 
		static void AddExistingMaterial(std::string accessName, TTN_Material::smatptr material);

		/////////////functions for accessing assets in the system/////////////////////
		//Gets a 2D texture pointer from the system 
		static TTN_Texture2D::st2dptr GetTexture2D(std::string accessName);
		//Gets a Skybox texture pointer from the system
		static TTN_TextureCubeMap::stcmptr GetSkybox(std::string accessName);
		//Gets a mesh pointer from the system
		static TTN_Mesh::smptr GetMesh(std::string accessName);
		//Gets a shader program pointer from the system
		static TTN_Shader::sshptr GetShader(std::string accessName);
		//Gets a material pointer from the system
		static TTN_Material::smatptr GetMaterial(std::string accessName);

		//functions to load a set of assets
		//loads all the assets in a set without breaking
		static void LoadSetNow(int set);
		//loads a set in the background, if a set is already being loaded, then it adds the new set to a loading queue 
		static void LoadSetInBackground(int set);

		//update function called from TTN_Application, the function that acutally executes on the loading in the background
		static void Update();

		//gets the number for the current set being loaded, or the set that has finished loading on the frame it was called, returns -1 if no set is being loaded
		static int GetCurrentSet();
		//gets whether the system finished loading a set this frame
		static bool GetFinishedLoadingSetThisFrame() { return s_FinishedLoadingSet; }

	private:
		//the current loading queue
		inline static std::vector<int> s_loadQueue = std::vector<int>();
		//the index of where the loader in it's current set of assets 
		inline static int s_CurrentAssetType = 0; //0 = 2D textures, 1 = cubemaps, 2 = non animated mesh, 3 = animated mesh, 4 = non-default shader, 5 = default shader
		inline static int s_CurrentAssetIndex = 0;
		//boolean that says it's finished loading a set on this frame
		inline static bool s_FinishedLoadingSet = false;
		//the 2 dimensional vector of strings for 2D textures to load
		inline static std::vector<std::vector<std::string>> s_2DTexturesToLoad = std::vector<std::vector<std::string>>();
		//the 2 dimensional vector of strings for cubemaps to load
		inline static std::vector<std::vector<std::string>> s_CubemapsToLoad = std::vector<std::vector<std::string>>();
		//the 2 dimensional vector of strings for non-animated meshes to load
		inline static std::vector<std::vector<std::string>> s_NonAnimatedMeshesToLoad = std::vector<std::vector<std::string>>();
		//the 2 dimensional vector of strings and ints for animated meshes to load 
		inline static std::vector<std::vector<std::pair<std::string, int>>> s_AnimatedMeshesToLoad = std::vector<std::vector<std::pair<std::string, int>>>();
		//the 2 dimension vector of sets of 3 string for non-default shaders to load
		inline static std::vector<std::vector<std::pair<std::string, std::pair<std::string, std::string>>>> s_ShadersToLoad =
			std::vector<std::vector<std::pair<std::string, std::pair<std::string, std::string>>>>();
		//the 2 dimensional vector of strings and sets of 2 defualt shader enums for default shaders to load
		inline static std::vector<std::vector<std::pair<std::string, std::pair<TTN_DefaultShaders, TTN_DefaultShaders>>>> s_DefaultShadersToLoad =
			std::vector<std::vector<std::pair<std::string, std::pair<TTN_DefaultShaders, TTN_DefaultShaders>>>>();


		//unordered maps to store the assets 
		//2D textures
		inline static std::unordered_map<std::string, TTN_Texture2D::st2dptr> s_texture2DMap = std::unordered_map<std::string, TTN_Texture2D::st2dptr>();
		//skyboxes
		inline static std::unordered_map<std::string, TTN_TextureCubeMap::stcmptr> s_cubemapMap = std::unordered_map<std::string, TTN_TextureCubeMap::stcmptr>();
		//meshes
		inline static std::unordered_map<std::string, TTN_Mesh::smptr> s_meshMap = std::unordered_map<std::string, TTN_Mesh::smptr>();
		//shaders
		inline static std::unordered_map<std::string, TTN_Shader::sshptr> s_shaderMap = std::unordered_map<std::string, TTN_Shader::sshptr>();
		//materials
		inline static std::unordered_map<std::string, TTN_Material::smatptr> s_matMap = std::unordered_map<std::string, TTN_Material::smatptr>();
	};
}