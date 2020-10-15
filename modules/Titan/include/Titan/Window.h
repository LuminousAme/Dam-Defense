#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GLM/glm.hpp"
#include <string>

namespace Titan {

	class Window
	{
	public:

		Window(int width, int height, const std::string& name);
		~Window();

		//static void Init(int width, int height, const std::string& name);


		static bool ShouldClose();




		static GLFWwindow* gwindow;  //the window	

	private:

		int height, width;
		const char* title;
		bool Init();



	};

}