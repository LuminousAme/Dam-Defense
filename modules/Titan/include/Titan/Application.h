//Titan Engine, by Atlas X Games 
// Application.h - header for the class that runs the program, creating the window, etc.
#pragma once

//include the required features and libraries 
#include "GLFW/glfw3.h"
#include "GLM/glm.hpp"
#include <string>
#include <vector>
//include the titan scene class 
#include "Scene.h"

namespace Titan {
	
	//class to represent the application, window, etc.
	class TTN_Application {
	public:
		//function to initilize the window
		static void Init(const std::string name, int width, int height);

		//gets wheter or not the application is closing 
		static bool GetIsClosing();

		//function for shutting down things when the application is closing 
		static void Closing();

		//function for starting a new frame 
		static void NewFrameStart();

		//function to update deltatime each frame
		static void NewDeltaTime();

		//function to get the change in time 
		static float GetDeltaTime();

		//function to set the background colour of the window
		static void SetClearColor(const glm::vec4& clearColor);

		//function to run through each frame, calling scene renders, etc.
		static void Update();

	public:
		//vector for all the scenes in the application
		std::vector<TTN_Scene> scenes;

	protected:
		//default constructor, just creates an empty aplication project
		TTN_Application() = default;

		static GLFWwindow* m_window;
		static float m_dt;
		static float m_previousFrameTime;
	};
}

