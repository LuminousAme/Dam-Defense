//Titan Engine, by Atlas X Games 
// Application.h - header for the class that runs the program, creating the window, etc.
#pragma once

//tell GLFW to not import OpenGL cause Glad imports it too.
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

//include the titan scene class 
#include "Scene.h"
//include the required features and libraries 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GLM/glm.hpp"
#include <string>
#include <vector>


namespace Titan {
	
	//class to represent the application, window, etc.
	class TTN_Application {
	public:
		//default destructor 
		~TTN_Application() = default;

		//function to initilize the window
		static void Init(const std::string name, int width, int height);

		//gets wheter or not the application is closing 
		static bool GetIsClosing();

		//function for shutting down things when the application is closing 
		static void Closing();

		//function for starting a new frame 
		static void NewFrameStart();

		//function to get the change in time 
		static float GetDeltaTime();

		//function to set the background colour of the window
		static void SetClearColor(const glm::vec4& clearColor);

		//function to run through each frame, calling scene renders, etc.
		static void Update();

	public:
		//vector for all the scenes in the application
		static std::vector<TTN_Scene> scenes;

	protected:
		//default constructor, just creates an empty aplication project
		TTN_Application() = default;

		static GLFWwindow* m_window;
		static float m_dt;
		static float m_previousFrameTime;
	};
}

