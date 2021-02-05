#include "Titan/LUT.h"
#pragma warning(disable : 4996)


 TTN_LUT3D:: TTN_LUT3D()
{
}

 TTN_LUT3D:: TTN_LUT3D(std::string path)
{
	loadFromFile(path);
}

void  TTN_LUT3D::loadFromFile(std::string path)
{
	std::string filePath = path;
	std::ifstream LUTstream;
	LUTstream.open(filePath);

	while (!LUTstream.eof())
	{
		std::string _line;
		std::getline(LUTstream, _line);

		if (_line.empty())
			continue;

		glm::vec3 lineData;
		if (sscanf(_line.c_str(), "%f %f %f", &lineData.x, &lineData.y, &lineData.z) == 3)
			data.push_back(lineData);
	}

	glEnable(GL_TEXTURE_3D);

	glGenTextures(1, &_handle);
	bind();
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, 64, 64, 64, 0, GL_RGB, GL_FLOAT, &data[0]);
	unbind();

	glDisable(GL_TEXTURE_3D);
}

void  TTN_LUT3D::bind()
{
	glBindTexture(GL_TEXTURE_3D, _handle);
}

void  TTN_LUT3D::unbind()
{
	glBindTexture(GL_TEXTURE_3D, GL_NONE);
}

void  TTN_LUT3D::bind(int textureSlot)
{
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	bind();
}

void  TTN_LUT3D::unbind(int textureSlot)
{
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	unbind();
}