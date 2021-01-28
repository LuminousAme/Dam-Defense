//Titan Engine, by Atlas X Games 
// AssetSystem.cpp - source file for the class that manages assets, allowing them to be loaded and accessed dynamically

//include the precompile header
#include "Titan/ttn_pch.h"
#include "Titan/AssetSystem.h"

namespace Titan {
	//adds a 2D texture to the list of assets to be loaded
	void TTN_AssetSystem::AddTexture2DToBeLoaded(std::string accessName, std::string fileName, int set) {
		//ensure the set is a atleast zero
		if (set < 0)
			LOG_ERROR("Asset Sets cannot be a number below 0");

		//first ensure that set acutally exists in memory
		if (s_2DTexturesToLoad.size() <= set)
			//if it doesn't create it
			s_2DTexturesToLoad.resize(set + 1);

		//then add the file and access name to the list of textures to be loaded in that set
		s_2DTexturesToLoad[set].push_back(std::pair(accessName, fileName));
	}

	//adds a cubemap to the list of assets to be loaded
	void TTN_AssetSystem::AddSkyboxToBeLoaded(std::string accessName, std::string fileName, int set) {
		//ensure the set is a atleast zero
		if (set < 0)
			LOG_ERROR("Asset Sets cannot be a number below 0");

		//first ensure that set acutally exists in memory
		if (s_CubemapsToLoad.size() <= set)
			//if it doesn't create it
			s_CubemapsToLoad.resize(set + 1);

		//then add the file and access name to the list of textures to be loaded in that set
		s_CubemapsToLoad[set].push_back(std::pair(accessName, fileName));
	}

	//adds a non-animated mesh to the list of assets to be loaded
	void TTN_AssetSystem::AddMeshToBeLoaded(std::string accessName, std::string fileName, int set) {
		//ensure the set is a atleast zero
		if (set < 0)
			LOG_ERROR("Asset Sets cannot be a number below 0");

		//first ensure that set acutally exists in memory
		if (s_NonAnimatedMeshesToLoad.size() <= set)
			//if it doesn't create it
			s_NonAnimatedMeshesToLoad.resize(set + 1);

		//then add the file and access name to the list of non-animated meshes to be loaded in that set
		s_NonAnimatedMeshesToLoad[set].push_back(std::pair(accessName, fileName));
	}

	//adds an animated mesh to the list of assets to be loaded
	void TTN_AssetSystem::AddMorphAnimationMeshesToBeLoaded(std::string accessName, std::string fileName, int NumOfFrames, int set) {
		//ensure the set is a atleast zero
		if (set < 0)
			LOG_ERROR("Asset Sets cannot be a number below 0");

		//first ensure that set acutally exists in memory
		if (s_AnimatedMeshesToLoad.size() <= set)
			//if it doesn't create it
			s_AnimatedMeshesToLoad.resize(set + 1);

		//then add the file and access name to the list of animated meshes to be loaded in that set
		s_AnimatedMeshesToLoad[set].push_back(std::pair(accessName, std::pair(fileName, NumOfFrames)));
	}

	//adds a non-default shader to the list of assets to be loeaded
	void TTN_AssetSystem::AddShaderToBeLoaded(std::string accessName, std::string VertShaderFileName, std::string FragShaderFileName, int set) {
		//ensure the set is a atleast zero
		if (set < 0)
			LOG_ERROR("Asset Sets cannot be a number below 0");

		//first ensure that set acutally exists in memory
		if (s_ShadersToLoad.size() <= set)
			//if it doesn't create it
			s_ShadersToLoad.resize(set + 1);

		//then add the file and access name to the list of non default shaders to be loaded in that set
		s_ShadersToLoad[set].push_back(std::pair(accessName, std::pair(VertShaderFileName, FragShaderFileName)));
	}

	//adds a default shader to the list of assets to be loaded
	void TTN_AssetSystem::AddDefaultShaderToBeLoaded(std::string accessName, TTN_DefaultShaders VertShader, TTN_DefaultShaders FragShader, int set) {
		//ensure the set is a atleast zero
		if (set < 0)
			LOG_ERROR("Asset Sets cannot be a number below 0");

		//first ensure that set acutally exists in memory
		if (s_DefaultShadersToLoad.size() <= set)
			//if it doesn't create it
			s_DefaultShadersToLoad.resize(set + 1);

		//then add the file and access name to the list of non default shaders to be loaded in that set
		s_DefaultShadersToLoad[set].push_back(std::pair(accessName, std::pair(VertShader, FragShader)));
	}

	//creates a new material pointer in the asset system
	void TTN_AssetSystem::CreateNewMaterial(std::string accessName) {
		//create a new material in the unordered map, keyed with the access name
		s_matMap[accessName] = TTN_Material::Create();
	}

	//adds an existing texture2D pointer to the system
	void TTN_AssetSystem::AddExisting2DTexture(std::string accessName, TTN_Texture2D::st2dptr texture) {
		//add the texture to the unordered map, keyed with the access name
		s_texture2DMap[accessName] = texture;
	}

	//adds an existing cubemap texture pointer to the system
	void TTN_AssetSystem::AddExistingSkybox(std::string accessName, TTN_TextureCubeMap::stcmptr cubeMap) {
		//add the cubemap texture to the unordered map, keyed with the access name
		s_cubemapMap[accessName] = cubeMap;
	}

	//adds an existing mesh pointer to the system
	void TTN_AssetSystem::AddExistingMesh(std::string accessName, TTN_Mesh::smptr mesh) {
		//adds the mesh to the unordered map, keyed with the access name
		s_meshMap[accessName] = mesh;
	}

	//adds an existing shader pointer to the system 
	void TTN_AssetSystem::AddExistingShader(std::string accessName, TTN_Shader::sshptr shader) {
		//adds the shader to the unordered map, keyed with the access name
		s_shaderMap[accessName] = shader;
	}

	//adds an existing material pointer to the system
	void TTN_AssetSystem::AddExistingMaterial(std::string accessName, TTN_Material::smatptr material) {
		//adds the material to the unordered map, keyed with the access name
		s_matMap[accessName] = material;
	}

	//gets a 2D texture pointer from the system
	TTN_Texture2D::st2dptr TTN_AssetSystem::GetTexture2D(std::string accessName) {
		//if there is a texture in that slot return it 
		if (s_texture2DMap.count(accessName))
			return s_texture2DMap[accessName];

		//otherwise return a nullpointer
		return nullptr;
	}

	//gets an existing cubemap texture poitner from the system
	TTN_TextureCubeMap::stcmptr TTN_AssetSystem::GetSkybox(std::string accessName) {
		//if there is a cubemap texture in that slot return it 
		if (s_cubemapMap.count(accessName))
			return s_cubemapMap[accessName];

		//otherwise return a nullpointer
		return nullptr;
	}

	//gets an existing mesh pointer from the system
	TTN_Mesh::smptr TTN_AssetSystem::GetMesh(std::string accessName) {
		//if there is a mesh in that slot return it 
		if (s_meshMap.count(accessName))
			return s_meshMap[accessName];

		//otherwise return a nullpointer
		return nullptr;
	}

	//gets an existing shader pointer from the system
	TTN_Shader::sshptr TTN_AssetSystem::GetShader(std::string accessName) {
		//if there is a shader in that slot return it 
		if (s_shaderMap.count(accessName))
			return s_shaderMap[accessName];

		//otherwise return a nullpointer
		return nullptr;
	}

	//gets an existing material pointer from the system
	TTN_Material::smatptr TTN_AssetSystem::GetMaterial(std::string accessName) {
		//if there is a material in that slot return it 
		if (s_matMap.count(accessName))
			return s_matMap[accessName];

		//otherwise return a nullpointer
		return nullptr;
	}

	//loads an entire set of assets at the time of the function call
	void TTN_AssetSystem::LoadSetNow(int set) {
		//confirm there are textures in the set 
		if (s_2DTexturesToLoad.size() > set) {
			//iterate through all the 2D textures in the set first
			for (auto it : s_2DTexturesToLoad[set]) {
				//load the texture into the unordered map
				s_texture2DMap[it.first] = TTN_Texture2D::LoadFromFile(it.second);
			}
		}

		//confirm there are cubemaps in the set 
		if (s_CubemapsToLoad.size() > set) {
			//iterate through all the cubemap textures in the set next
			for (auto it : s_CubemapsToLoad[set]) {
				//load the cubemap into the unordered map
				s_cubemapMap[it.first] = TTN_TextureCubeMap::LoadFromImages(it.second);
			}
		}

		//confirm there are non animated meshes in the set 
		if (s_NonAnimatedMeshesToLoad.size() > set) {
			//iterate through all the non-animated meshes in the set 
			for (auto it : s_NonAnimatedMeshesToLoad[set]) {
				//load the mesh into the unordered map
				s_meshMap[it.first] = TTN_ObjLoader::LoadFromFile(it.second);
				s_meshMap[it.first]->SetUpVao();
			}
		}

		//confirm there are animated meshes in the set 
		if (s_AnimatedMeshesToLoad.size() > set) {
			//iterate through all the animated meshes in the set 
			for (auto it : s_AnimatedMeshesToLoad[set]) {
				//load the animated mesh into the unordered map
				s_meshMap[it.first] = TTN_ObjLoader::LoadAnimatedMeshFromFiles(it.second.first, it.second.second);
				s_meshMap[it.first]->SetUpVao();
			}
		}

		//confirm there are non default shaders in the set
		if (s_ShadersToLoad.size() > set) {
			//iterate through all the non default shaders in the set
			for (auto it : s_ShadersToLoad[set]) {
				//create and load the shader
				TTN_Shader::sshptr temp = TTN_Shader::Create();
				temp->LoadShaderStageFromFile(it.second.first.c_str(), GL_VERTEX_SHADER);
				temp->LoadShaderStageFromFile(it.second.second.c_str(), GL_FRAGMENT_SHADER);
				temp->Link();
				//and store it in the unordered map
				s_shaderMap[it.first] = temp;
			}
		}

		//confirm there are default shaders in the set
		if (s_DefaultShadersToLoad.size() > set) {
			//iterate through all the non default shaders in the set
			for (auto it : s_DefaultShadersToLoad[set]) {
				//create and load the shader
				TTN_Shader::sshptr temp = TTN_Shader::Create();
				temp->LoadDefaultShader(it.second.first);
				temp->LoadDefaultShader(it.second.second);
				temp->Link();
				//and store it in the unordered map
				s_shaderMap[it.first] = temp;
			}
		}
	}

	//load a set of assets in the background, loading 1 asset per frame as the program does other things
	void TTN_AssetSystem::LoadSetInBackground(int set) {
		//add the set to be loaded to the load queue
		s_loadQueue.push_back(set);
	}

	//the update function, run every frame, acutally does the background loading
	void TTN_AssetSystem::Update() {
		//check if a set has finished
		if (s_FinishedLoadingSet) {
			//remove the first element in the queue
			s_loadQueue.erase(s_loadQueue.begin());
			//and remove the finished flag
			s_FinishedLoadingSet = false;
		}

		//check there's a set in the queue
		if (s_loadQueue.size() > 0) {
			//if there is, we can load an asset

			//increment the current asset index
			s_CurrentAssetIndex++;

			//if we're currently loading 2D textures
			if (s_CurrentAssetType == 0) {
				//first confirm there are textures in the set
				if (s_2DTexturesToLoad.size() > s_loadQueue[0]) {
					//check the index is not out of bounds
					if (s_CurrentAssetIndex < s_2DTexturesToLoad[s_loadQueue[0]].size()) {
						//load the asset
						s_texture2DMap[s_2DTexturesToLoad[s_loadQueue[0]][s_CurrentAssetIndex].first] =
							TTN_Texture2D::LoadFromFile(s_2DTexturesToLoad[s_loadQueue[0]][s_CurrentAssetIndex].second);
					}
					//if it is, then move onto cubemaps
					else {
						s_CurrentAssetType = 1;
						s_CurrentAssetIndex = -1;
					}
				}
				//if there aren't, move onto cubemaps
				else {
					s_CurrentAssetType = 1;
					s_CurrentAssetIndex = -1;
				}
			}

			//if we're currently loading cubemaps
			else if (s_CurrentAssetType == 1) {
				//first confirm there are cubemaps in the set
				if (s_CubemapsToLoad.size() > s_loadQueue[0]) {
					//check the index is not out of bounds
					if (s_CurrentAssetIndex < s_CubemapsToLoad[s_loadQueue[0]].size()) {
						//load the asset
						s_cubemapMap[s_CubemapsToLoad[s_loadQueue[0]][s_CurrentAssetIndex].first] =
							TTN_TextureCubeMap::LoadFromImages(s_CubemapsToLoad[s_loadQueue[0]][s_CurrentAssetIndex].second);
					}
					//if it is, then move onto non animated meshes
					else {
						s_CurrentAssetType = 2;
						s_CurrentAssetIndex = -1;
					}
				}
				//if there aren't, move onto non animated meshes
				else {
					s_CurrentAssetType = 2;
					s_CurrentAssetIndex = -1;
				}
			}

			//if we're currently loading non animated meshes
			else if (s_CurrentAssetType == 2) {
				//first confirm there are meshes in the set
				if (s_NonAnimatedMeshesToLoad.size() > s_loadQueue[0]) {
					//check the index is not out of bounds
					if (s_CurrentAssetIndex < s_NonAnimatedMeshesToLoad[s_loadQueue[0]].size()) {
						//load the asset
						s_meshMap[s_NonAnimatedMeshesToLoad[s_loadQueue[0]][s_CurrentAssetIndex].first] =
							TTN_ObjLoader::LoadFromFile(s_NonAnimatedMeshesToLoad[s_loadQueue[0]][s_CurrentAssetIndex].second);

						s_meshMap[s_NonAnimatedMeshesToLoad[s_loadQueue[0]][s_CurrentAssetIndex].first]->SetUpVao();
					}
					//if it is, then move onto animated meshes
					else {
						s_CurrentAssetType = 3;
						s_CurrentAssetIndex = -1;
					}
				}
				//if there aren't, move onto animated meshes
				else {
					s_CurrentAssetType = 3;
					s_CurrentAssetIndex = -1;
				}
			}

			//if we're currently loading animated meshes
			else if (s_CurrentAssetIndex == 3) {
				//first confirm there are meshes in the set
				if (s_AnimatedMeshesToLoad.size() > s_loadQueue[0]) {
					//check the index is not out of bounds
					if (s_CurrentAssetIndex < s_AnimatedMeshesToLoad[s_loadQueue[0]].size()) {
						//load the asset
						s_meshMap[s_AnimatedMeshesToLoad[s_loadQueue[0]][s_CurrentAssetIndex].first] =
							TTN_ObjLoader::LoadAnimatedMeshFromFiles(s_AnimatedMeshesToLoad[s_loadQueue[0]][s_CurrentAssetIndex].second.first,
								s_AnimatedMeshesToLoad[s_loadQueue[0]][s_CurrentAssetIndex].second.second);

						s_meshMap[s_AnimatedMeshesToLoad[s_loadQueue[0]][s_CurrentAssetIndex].first]->SetUpVao();
					}
					//if it is, then move onto non-default shaders
					else {
						s_CurrentAssetType = 4;
						s_CurrentAssetIndex = -1;
					}
				}
				//if there aren't, move onto non-default shaders
				else {
					s_CurrentAssetType = 4;
					s_CurrentAssetIndex = -1;
				}
			}

			//if we're currently loading non-default shaders
			else if (s_CurrentAssetType == 4) {
				//first confirm there are shaders in the set
				if (s_ShadersToLoad.size() > s_loadQueue[0]) {
					//check the index is not out of bounds
					if (s_CurrentAssetIndex < s_ShadersToLoad[s_loadQueue[0]].size()) {
						//load the asset
						TTN_Shader::sshptr temp = TTN_Shader::Create();
						temp->LoadShaderStageFromFile(s_ShadersToLoad[s_loadQueue[0]][s_CurrentAssetIndex].second.first.c_str(), GL_VERTEX_SHADER);
						temp->LoadShaderStageFromFile(s_ShadersToLoad[s_loadQueue[0]][s_CurrentAssetIndex].second.second.c_str(), GL_FRAGMENT_SHADER);
						temp->Link();
						//and store it in the unordered map
						s_shaderMap[s_ShadersToLoad[s_loadQueue[0]][s_CurrentAssetIndex].first] = temp;
					}
					//if it is, then move onto non-default shaders
					else {
						s_CurrentAssetType = 5;
						s_CurrentAssetIndex = -1;
					}
				}
				//if there aren't, move onto non-default shaders
				else {
					s_CurrentAssetType = 5;
					s_CurrentAssetIndex = -1;
				}
			}

			//if we're currently loading default shaders
			else if (s_CurrentAssetType == 5) {
				//first confirm there are shader in the set
				if (s_DefaultShadersToLoad.size() > s_loadQueue[0]) {
					//check the index is not out of bounds
					if (s_CurrentAssetIndex < s_DefaultShadersToLoad[s_loadQueue[0]].size()) {
						//load the asset
						TTN_Shader::sshptr temp = TTN_Shader::Create();
						temp->LoadDefaultShader(s_DefaultShadersToLoad[s_loadQueue[0]][s_CurrentAssetIndex].second.first);
						temp->LoadDefaultShader(s_DefaultShadersToLoad[s_loadQueue[0]][s_CurrentAssetIndex].second.second);
						temp->Link();
						//and store it in the unordered map
						s_shaderMap[s_DefaultShadersToLoad[s_loadQueue[0]][s_CurrentAssetIndex].first] = temp;
					}
					//if it is, then move onto non-default shaders
					else {
						s_CurrentAssetType = 0;
						s_CurrentAssetIndex = -1;
						s_FinishedLoadingSet = true;
					}
				}
				//if there aren't, move onto non-default shaders
				else {
					s_CurrentAssetType = 0;
					s_CurrentAssetIndex = -1;
					s_FinishedLoadingSet = true;
				}
			}

		}
	}

	//gets the number for the current set being loaded, or the set that has finished loading on the frame it was called, returns -1 if no set is being loaded
	int TTN_AssetSystem::GetCurrentSet() {
		//if there's a set being loaded, return it's number
		if (s_loadQueue.size() > 0)
			return s_loadQueue[0];

		//otherwise return -1
		return -1;
	}
}