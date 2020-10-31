//Titan Engine, by Atlas X Games
//ObjLoader.cpp - source file for the class that parses OBJ files into TTN_Models 

//include the header
#include "Titan/ObjLoader.h"
//include other required features
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>

namespace Titan {

	// Taken from https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
#pragma region String Trimming

// trim from start (in place)
	static inline void ltrim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
		}));
	}

	// trim from end (in place)
	static inline void rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

	// trim from both ends (in place)
	static inline void trim(std::string& s) {
		ltrim(s);
		rtrim(s);
	}

#pragma endregion 

	TTN_Mesh::smptr TTN_ObjLoader::LoadFromFile(const std::string& fileName)
	{
		//Vectors for storing data parsed in 
		std::vector<glm::vec3> vertexPos;
		std::vector<glm::vec3> vertexUV; //stored in a vec3 because of the implementation of our VAO class, z value should be set to 0.0f
		std::vector<glm::vec3> vertexNorms; 

		std::vector<GLint> vertexPosIndices; 
		std::vector<GLint> vertexUvsIndices;
		std::vector<GLint> vertexNormIndices;

		//Open the file
		std::ifstream file;
		file.open(fileName, std::ios::binary);

		//if it fails to open, throw an error
		if (!file) {
			throw std::runtime_error("Obj Loader failed to open file.");
		}

		//get a string to represent each line of the file
		std::string line;

		//parse each line of the file
		while (std::getline(file, line)) {
			//trim the line so there isn't any white space
			trim(line);

			//check if the line is a comment
			if (line.substr(0, 1) == "#") {
				//if it is, there is no need to do anything with it
			}
			//if it's not a a comment, check if it is an object
			else if (line.substr(0, 1) == "o") {

			}
			//if not, then check if it's a vertex
			else if (line.substr(0, 2) == "v ") {
				//grab the line passed the v
				std::istringstream ss = std::istringstream(line.substr(2));
				//add a new vertex position to the vector
				vertexPos.push_back(glm::vec3(1.0f));
				//copy the line into it 
				ss >> vertexPos[vertexPos.size() - 1].x >> vertexPos[vertexPos.size() - 1].y >> vertexPos[vertexPos.size() - 1].z;
			}
			//if not then check if it's a uv
			else if (line.substr(0, 2) == "vt") {
				//grab the line past the vt
				std::istringstream ss = std::istringstream(line.substr(2));
				//add a new vertex uv to the vector 
				vertexUV.push_back(glm::vec3(1.0f));
				//copy the line into it
				ss >> vertexUV[vertexUV.size() - 1].x >> vertexUV[vertexUV.size() - 1].y;
				//set the z to 0
				vertexUV[vertexUV.size() - 1].z = 0.0f;
			}
			//if not then check if it's a normal
			else if (line.substr(0, 2) == "vn") {
				//grab the line passed the vn
				std::istringstream ss = std::istringstream(line.substr(2));
				//add a new vertex normal to the vector
				vertexNorms.push_back(glm::vec3(1.0f));
				//copy the line into it 
				ss >> vertexNorms[vertexNorms.size() - 1].x >> vertexNorms[vertexNorms.size() - 1].y >> vertexNorms[vertexNorms.size() - 1].z;
			}
			//if not then check if it's a face index detail
			else if (line.substr(0, 1) == "f") {
				int counter = 0; //used to check which detail we're on (0=vertex index, 1=uv index, 2=normal index)
				GLint temp; //used to temporarily store the index before pushing it into one of the vectors
				std::istringstream ss = std::istringstream(line.substr(1));
				//go through the line
				while (ss >> temp) {
					if (counter == 0) vertexPosIndices.push_back(temp);
					else if (counter == 1) vertexUvsIndices.push_back(temp);
					else if (counter == 2) vertexNormIndices.push_back(temp);

					//check if the next character is a forward slash
					if (ss.peek() == '/') {
						//if it is, move to the next character, skipping over the slash
						counter++;
						ss.ignore(1, '/');
					}
					//if it's not, check if it's a space
					else if (ss.peek() == ' ') {
						//if it is, move to the next character, skipping over the space
						counter++;
						ss.ignore(1, ' ');
					}

					//if the counter goes above 2, reset it to zero so it takesd another vertex position
					if (counter > 2)
						counter = 0;
				}
					
			}
			//if it's anything else we can just ignore it for now
		}

		//now we have loaded in all the data, we can use it to construct a mesh 
		//create a new vectors with all the positions, uvs, and normals (this is the lenght of the indices, as these are all the vertices needed to construct the faces, some of which
		//may have the same values twice) 
		std::vector <glm::vec3> meshVertPos; 
		std::vector <glm::vec3> meshVertUvs;
		std::vector <glm::vec3> meshVertNorms;
		//make that those vector the lenght of the their indices vector equilavents
		meshVertPos.resize(vertexPosIndices.size(), glm::vec3());
		meshVertUvs.resize(vertexUvsIndices.size(), glm::vec3());
		meshVertNorms.resize(vertexNormIndices.size(), glm::vec3());
		
		//copy the data from the first set of vectors into the new vectors based on the indices so that they can used by the mesh
		for (int i = 0; i < meshVertPos.size(); i++) {
			//copy the positions
			meshVertPos[i] = vertexPos[vertexPosIndices[i] - 1]; 
		}
		for (int i = 0; i < meshVertUvs.size(); i++) {
			//copy the uvs
			meshVertUvs[i] = vertexUV[vertexUvsIndices[i] - 1];
		}
		for (int i = 0; i < meshVertNorms.size(); i++) {
			//copy the uvs
			meshVertNorms[i] = vertexNorms[vertexNormIndices[i] - 1];
		}

		//create and return the mesh from that data
		TTN_Mesh::smptr newMesh = TTN_Mesh::Create();
		newMesh->setVertices(meshVertPos);
		newMesh->setNormals(meshVertNorms);
		newMesh->setUVs(meshVertUvs);

		return newMesh;
	}
}