//Titan Engine, by Atlas X Games 
// AssetSystem.h - header for the class that manages assets, allowing them to be loaded and accessed dynamically
#pragma once

//include the precompile header with a bunch of system
#include "Titan/ttn_pch.h"
//include all of the various asset classes and their loaders
#include "Titan/Graphics/Texture2D.h"
#include "Titan/Graphics/TextureCubeMap.h"
#include "Titan/Graphics/Mesh.h"
#include "Titan/Graphics/ObjLoader.h"
#include "Titan/Graphics/Shader.h"
#include "Titan/Graphics/Material.h"
#include "Titan/Graphics/Post/LUT.h"

namespace Titan {
	//class to control all the assets in any given project
	class TTN_AssetSystem {
	public:
		//default destructor
		~TTN_AssetSystem() = default;

		/////////////functions for loading assets/////////////////////
		//Adds a 2D texture to the list of assets to be loaded
		static void AddTexture2DToBeLoaded(std::string accessName, std::string fileName, int set = 0);
		//Adds a Skybox to the list of assets to be loaded
		static void AddSkyboxToBeLoaded(std::string accessName, std::string fileName, int set = 0);
		//Adds a mesh to the list of assets to be loaded
		static void AddMeshToBeLoaded(std::string accessName, std::string fileName, int set = 0);
		//Adds a morph animation mesh to the list of assets to be loaded
		static void AddMorphAnimationMeshesToBeLoaded(std::string accessName, std::string fileName, int NumOfFrames, int set = 0);
		//Adds a shader to the list of assets to be loaded
		static void AddShaderToBeLoaded(std::string accessName, std::string VertShaderFileName, std::string FragShaderFileName, int set = 0);
		//Adds a Titan default shader to be loaded
		static void AddDefaultShaderToBeLoaded(std::string accessName, TTN_DefaultShaders VertShader, TTN_DefaultShaders FragShader, int set = 0);
		//Creates a new material pointer in the system 
		static void CreateNewMaterial(std::string accessName);
		//Adds a 3D cube look up table to be loaded
		static void AddLUTTobeLoaded(std::string accessName, std::string fileName, int set = 0);

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
		//Adds an already existing cube look up table
		static void AddExistingLUT(std::string accessName, TTN_LUT3D::sltptr cube);

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
		//Gets a lut pointer from the system
		static TTN_LUT3D::sltptr GetLUT(std::string accessName);

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
		static bool GetSetLoaded(int set) { return s_setsLoaded[set]; }

	private:
		//structures for storing asset loading data
		struct AccessAndFileName {
			std::string m_AccessName;
			std::string m_FileName;

			AccessAndFileName(std::string accessName, std::string fileName)
			{
				m_AccessName = accessName;
				m_FileName = fileName;
			}
		};

		struct AccessNameFileNameAndNumber {
			std::string m_AccessName;
			std::string m_FileName;
			int m_number;

			AccessNameFileNameAndNumber(std::string accessName, std::string fileName, int number)
			{
				m_AccessName = accessName;
				m_FileName = fileName;
				m_number = number;
			}
		};

		struct AccessNameAndTwoShaderFiles {
			std::string m_AccessName;
			std::string m_vertShader;
			std::string m_fragShader;

			AccessNameAndTwoShaderFiles(std::string accessName, std::string vertex_shader, std::string fragmentShader)
			{
				m_AccessName = accessName;
				m_vertShader = vertex_shader;
				m_fragShader = fragmentShader;
			}
		};

		struct AccessNameAndTwoDefaultShaders {
			std::string m_AccessName;
			TTN_DefaultShaders m_vertShader;
			TTN_DefaultShaders m_fragShader;

			AccessNameAndTwoDefaultShaders(std::string accessName, TTN_DefaultShaders vertex_shader, TTN_DefaultShaders fragmentShader)
			{
				m_AccessName = accessName;
				m_vertShader = vertex_shader;
				m_fragShader = fragmentShader;
			}
		};

	private:
		//the current loading queue
		inline static std::vector<int> s_loadQueue = std::vector<int>();
		//sets that have been loaded
		inline static std::unordered_map<int, bool> s_setsLoaded = std::unordered_map<int, bool>();
		//the index of where the loader in it's current set of assets 
		inline static int s_CurrentAssetType = 0; //0 = 2D textures, 1 = cubemaps, 2 = non animated mesh, 3 = animated mesh, 4 = non-default shader, 5 = default shader
		inline static int s_CurrentAssetIndex = -1;
		//boolean that says it's finished loading a set on this frame
		inline static bool s_FinishedLoadingSet = false;

		//the map of vector of strings for 2D textures to load
		inline static std::unordered_map<int, std::vector<AccessAndFileName>> s_2DTexturesToLoad = std::unordered_map<int, std::vector<AccessAndFileName>>();
		//the map of vector of strings  for cubemaps to load
		inline static std::unordered_map<int, std::vector<AccessAndFileName>> s_CubemapsToLoad = std::unordered_map<int, std::vector<AccessAndFileName>>();
		//the map of vector of strings for non-animated meshes to load
		inline static std::unordered_map<int, std::vector<AccessAndFileName>> s_NonAnimatedMeshesToLoad = std::unordered_map<int, std::vector<AccessAndFileName>>();
		//the map of strings and ints for animated meshes to load 
		inline static std::unordered_map<int, std::vector<AccessNameFileNameAndNumber>> s_AnimatedMeshesToLoad = std::unordered_map<int, std::vector<AccessNameFileNameAndNumber>>();
		//the map of sets of 3 string for non-default shaders to load
		inline static std::unordered_map<int, std::vector<AccessNameAndTwoShaderFiles>> s_ShadersToLoad = std::unordered_map<int, std::vector<AccessNameAndTwoShaderFiles>>();
		//the map of strings and sets of 2 defualt shader enums for default shaders to load
		inline static std::unordered_map<int, std::vector<AccessNameAndTwoDefaultShaders>> s_DefaultShadersToLoad = std::unordered_map<int, std::vector<AccessNameAndTwoDefaultShaders>>();
		//the map of vector of strings for cube look up tables to load
		inline static std::unordered_map<int, std::vector<AccessAndFileName>> s_LUTsToLoad = std::unordered_map<int, std::vector<AccessAndFileName>>();

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
		//luts
		inline static std::unordered_map<std::string, TTN_LUT3D::sltptr> s_LUTMap = std::unordered_map<std::string, TTN_LUT3D::sltptr>();
	};
}