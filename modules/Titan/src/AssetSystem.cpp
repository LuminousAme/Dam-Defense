//Titan Engine, by Atlas X Games 
// AssetSystem.cpp - source file for the class that manages assets, allowing them to be loaded and accessed dynamically

//include the precompile header
#include "Titan/ttn_pch.h"
#include "..\include\Titan\AssetSystem.h"

namespace Titan {
	//adds a 2D texture to the list of assets to be loaded
	void TTN_AssetSystem::AddTexture2DToBeLoaded(std::string fileName, int set)
	{
	}

	//adds a cubemap to the list of assets to be loaded
	void TTN_AssetSystem::AddSkyboxToBeLoaded(std::string fileName, int set)
	{
	}

	//adds a non-animated mesh to the list of assets to be loaded
	void TTN_AssetSystem::AddMeshToBeLoaded(std::string fileName, int set)
	{
	}
	
	//adds an animated mesh to the list of assets to be loaded
	void TTN_AssetSystem::AddMorphAnimationMeshesToBeLoaded(std::string fileName, int NumOfFrames, int set)
	{
	}
	
	//adds a non-default shader to the list of assets to be loeaded
	void TTN_AssetSystem::AddShaderToBeLoaded(std::string accessName, std::string VertShaderFileName, std::string FragShaderFileName, int set)
	{
	}

	//adds a default shader to the list of assets to be loaded
	void TTN_AssetSystem::AddDefaultShaderToBeLoaded(std::string accessName, TTN_DefaultShaders VertShader, TTN_DefaultShaders FragShader, int set)
	{
	}

	//creates a new material pointer in the asset system
	void TTN_AssetSystem::CreateNewMaterial(std::string accessName)
	{
	}

	//adds an existing texture2D pointer to the system
	void TTN_AssetSystem::AddExisting2DTexture(std::string accessName, TTN_Texture2D::st2dptr texture)
	{
	}

	//adds an existing cubemap texture pointer to the system
	void TTN_AssetSystem::AddExistingSkybox(std::string accessName, TTN_TextureCubeMap::stcmptr cubeMap)
	{
	}

	//adds an existing mesh pointer to the system
	void TTN_AssetSystem::AddExistingMesh(std::string accessName, TTN_Mesh::smptr mesh)
	{
	}

	//adds an existing shader pointer to the system 
	void TTN_AssetSystem::AddExistingShader(std::string accessName, TTN_Shader::sshptr shader)
	{
	}

	//adds an existing material pointer to the system
	void TTN_AssetSystem::AddExistingMaterial(std::string accessName, TTN_Material::smatptr material)
	{
	}

	//gets a 2D texture pointer from the system
	TTN_Texture2D::st2dptr TTN_AssetSystem::GetTexture2D(std::string accessName)
	{
		return TTN_Texture2D::st2dptr();
	}

	//gets an existing cubemap texture poitner from the system
	TTN_TextureCubeMap::stcmptr TTN_AssetSystem::GetSkybox(std::string accessName)
	{
		return TTN_TextureCubeMap::stcmptr();
	}

	//gets an existing mesh pointer from the system
	TTN_Mesh::smptr TTN_AssetSystem::GetMesh(std::string accessName)
	{
		return TTN_Mesh::smptr();
	}

	//gets an existing shader pointer from the system
	TTN_Shader::sshptr TTN_AssetSystem::GetShader(std::string accessName)
	{
		return TTN_Shader::sshptr();
	}

	//gets an existing material pointer from the system
	TTN_Material::smatptr TTN_AssetSystem::GetMaterial(std::string accessName)
	{
		return TTN_Material::smatptr();
	}

	//loads an entire set of assets at the time of the function call
	void TTN_AssetSystem::LoadSetNow(int set)
	{
	}

	//load a set of assets in the background, loading 1 asset per frame as the program does other things
	void TTN_AssetSystem::LoadSetInBackground(int set)
	{
	}

	//the update function, run every frame, acutally does the background loading
	void TTN_AssetSystem::Update()
	{
	}

	//gets the current set being loaded
	int TTN_AssetSystem::GetCurrentSet()
	{
		return 0;
	}
}