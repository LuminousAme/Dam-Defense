#include "OBJLoader.h"


#include <string>
#include <sstream>
#include <fstream>
#include <iostream>


void LoadFile(const std::string& filename){

	// Open our file in binary mode
	std::ifstream file;
	file.open(filename, std::ios::binary);

	// If our file fails to open, we will throw an error
	if (!file) {
		throw std::runtime_error("Failed to open file");
	}


}
