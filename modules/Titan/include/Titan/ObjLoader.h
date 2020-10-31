//Titan Engine, by Atlas X Games
//ObjLoader.h - header file for the class that parses OBJ files into TTN_Models 
#pragma once

//include the mesh class so we write the data to it 
#include "Mesh.h"

namespace Titan {
	
	//class to parse ObjFiles into TTN_Model objects
	class TTN_ObjLoader {
	public:
		static TTN_Mesh::smptr LoadFromFile(const std::string& fileName);

	protected:
		TTN_ObjLoader() = default;
		~TTN_ObjLoader() = default;
	};
}
