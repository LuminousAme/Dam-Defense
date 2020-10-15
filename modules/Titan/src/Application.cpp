//Titan Engine, by Atlas X Games 
// Application.cpp - source file for the class that runs the program, creating the window, etc.

//include the header 
#include "Titan/Application.h"
//import other required features
#include <stdio.h>

namespace Titan {
	//set the base values for the member variables
	GLFWwindow* TTN_Application::m_window = nullptr;
	float TTN_Application::m_dt = 0.0f;
	float TTN_Application::m_previousFrameTime = 0.0f;

	//function to initialize a new window 
	void TTN_Application::Init(const std::string name, int width, int height)
	{
		//init GLFW and check it initliazed properly 
		if (glfwInit() == GLFW_FALSE)
		{
			//if it did not init properly print that to the console and throw a runtime error
			printf("GLFW init failed.");
			throw std::runtime_error("GLFW init failed");
		}


		//initliaze glad and check it initliazed properly 
		if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
			//if it did not init properly print that to the console and throw a runtime error
			printf("glad init failed.");
			throw std::runtime_error("glad init failed");
		}

		//prevent the user from resizing the window at will (it could distort our graphics if they could)
		glfwWindowHint(GLFW_RESIZABLE, false);

		//create the window 
		m_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

		//set the window we want to draw on to the window that was just created
		glfwMakeContextCurrent(m_window);

		//enable depth test so things don't get drawn on top of objects behind them 
		glEnable(GL_DEPTH_TEST);

		//enable cull faces so only the front faces are rendered, this will improve performance and model back faces shouldn't be
		//visible anyways
		glEnable(GL_CULL_FACE);

		//Set the background colour for our scene to the base black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}

	//function to check if the window is being closed
	bool TTN_Application::GetIsClosing()
	{
		//have glfw check if the user has tried to close the window, return what it says
		return glfwWindowShouldClose(m_window);
	}

	//function that cleans things up when the window closes so there are no memory leaks and everything goes cleanly 
	void TTN_Application::Closing()
	{
		//have glfw destroy the window 
		glfwDestroyWindow(m_window);
		//close glfw
		glfwTerminate();
	}

	void TTN_Application::NewFrameStart()
	{
		//Find deltatime for the frame 
		//first grab the current time from glfw
		float Currenttime = (float)glfwGetTime();
		//calculate deltatime by subtracting the time at the last frame
		m_dt = Currenttime - m_previousFrameTime;
		//save time in the previous frame time variable so we can calculate deltatime correctly next frame 
		m_previousFrameTime = Currenttime;

		//Clear our window 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	//function to get the delta time so it can be used for other operations and systems
	float TTN_Application::GetDeltaTime()
	{
		return m_dt;
	}

	//function to set the background colour that the window clears to 
	void TTN_Application::SetClearColor(const glm::vec4& clearColor)
	{
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	}

	//function to run each frame 
	void TTN_Application::Update()
	{
		//start a new frame 
		TTN_Application::NewFrameStart();

		//go through each scene 
		for (int i = 0; i < TTN_Application::scenes.size(); i++) {
			//and check if they should be rendered
			if (TTN_Application::scenes[i].GetShouldRender()) {
				//if they should, then update and render them 
				TTN_Application::scenes[i].Update(m_dt);
				TTN_Application::scenes[i].Render();
			}
		}

		//now all the scenes that should be rendered (current gameplay scene, ui, etc.) will be rendered
		//while anything that doesn't need to be rendered (such as a prefabs scene) will not 
		
		//swap the buffers so all the drawings that the scenes just did are acutally visible 
		glfwSwapBuffers(m_window);
	}
}