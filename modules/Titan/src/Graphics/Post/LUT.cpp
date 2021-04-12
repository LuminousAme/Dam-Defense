//Titan Engine by Atlas X Games
//LUT.cpp - source file for the class that represents 3D look up tables for color correction

//include the precompile header
#include "Titan/ttn_pch.h"
//include the class
#include "Titan/Graphics/Post/LUT.h"

//disable some compiler warnings
#pragma warning(disable : 4996)

namespace Titan {
	//default constructor
	TTN_LUT3D::TTN_LUT3D()
	{
	}

	//constructor that takes a path to a .cube file and loads it 
	TTN_LUT3D::TTN_LUT3D(std::string path)
	{
		//load the cube file
		loadFromFile(path);
	}

	//function to load in a .cube file from a file path
	void TTN_LUT3D::loadFromFile(std::string path)
	{
		//open the file
		std::string filePath = path;
		std::ifstream LUTstream;
		LUTstream.open(filePath);

		//loop through every line of the file
		while (!LUTstream.eof())
		{
			//get each line
			std::string _line;
			std::getline(LUTstream, _line);

			//if the line is empty just skip to the next line
			if (_line.empty())
				continue;

			//if it has data extract and store it
			glm::vec3 lineData;
			if (sscanf(_line.c_str(), "%f %f %f", &lineData.x, &lineData.y, &lineData.z) == 3)
				data.push_back(lineData);
		}

		//create a 3D texture for the cube
		glEnable(GL_TEXTURE_3D);
		//generate the handle, bind it and set the texture parameters
		glGenTextures(1, &m_handle);
		bind();
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		//load the data into the openGL texture
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, 64, 64, 64, 0, GL_RGB, GL_FLOAT, &data[0]);
		//unbind it and stop creating 3D textures
		unbind();
		glDisable(GL_TEXTURE_3D);
	}

	//bind the look up table as a 3D texture
	void TTN_LUT3D::bind()
	{
		glBindTexture(GL_TEXTURE_3D, m_handle);
	}

	//unbind the texture
	void TTN_LUT3D::unbind()
	{
		glBindTexture(GL_TEXTURE_3D, GL_NONE);
	}

	//bidn the texture to a specfic slot
	void TTN_LUT3D::bind(int textureSlot)
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		bind();
	}

	//unbind the texture from a specfic slot
	void TTN_LUT3D::unbind(int textureSlot)
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		unbind();
	}
}