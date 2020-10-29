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
	std::vector<TTN_Scene> TTN_Application::scenes = std::vector<TTN_Scene>();
	std::unordered_map<TTN_KeyCode, bool> TTN_Application::TTN_Input::KeyWasPressedMap;
	std::unordered_map<TTN_MouseButton, bool> TTN_Application::TTN_Input::MouseWasPressedMap;
	glm::vec2 TTN_Application::TTN_Input::mousePos = glm::vec2(0.0f);
	bool TTN_Application::TTN_Input::inFrame = false;

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

		//prevent the user from resizing the window at will (it could distort our graphics if they could)
		glfwWindowHint(GLFW_RESIZABLE, false);

		//create the window 
		m_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

		//set the window we want to draw on to the window that was just created
		glfwMakeContextCurrent(m_window);

		//initliaze glad and check it initliazed properly 
		if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
			//if it did not init properly print that to the console and throw a runtime error
			printf("glad init failed.");
			throw std::runtime_error("glad init failed");
		}

		//set the cursor callbacks so we can get the cursor position
		glfwSetCursorEnterCallback(m_window, TTN_Input::cursorEnterFrameCallback);

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

		//check for events from glfw 
		glfwPollEvents();

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

	//checks if a key is being pressed
	bool TTN_Application::TTN_Input::GetKey(TTN_KeyCode key)
	{
		//check if the keyinputted currently exists in the map
		if (KeyWasPressedMap.find(key) == KeyWasPressedMap.end())
		{
			//if it doesn't, add it to the map (doing this here means we only add keys to the map if the user acutally wants to use them) 
			KeyWasPressedMap[key] = false;
		}
		//check if the key has been pressed
		if (glfwGetKey(m_window, static_cast<int>(key)))
		{
			//if it has, set it's place in the map to true
			KeyWasPressedMap.at(key) = true;
			//return true as the key is being pressed
			return true;
		}
		//if it hasn't been pressed, return false
		return false;
	}

	//checks if this is the first frame a key is being pressed
	bool TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode key)
	{
		//check first if the key exists in the map
		if (KeyWasPressedMap.find(key) == KeyWasPressedMap.end())
		{
			//if it doesn't, pass it onto the getKey so it can create a place in the map, and check if it is being pressed
			if (GetKey(key))
			{
				//if it returns true that means the key has been pressed this frame
				return true;
			}
		}
		//if it does exist in the map, check if the value in the map is false and if the key is being pressed
		else if (!KeyWasPressedMap.at(key) && GetKey(key))
			//if it is, then this is the first frame where it's being pressed so return true
			return true;

		//if none of those are true, then either the key isn't being pressed, or this isn't the first frame it's being pressed
		//either way, return false
		return false;
	}

	//checks if a key was pressed down, but has now been released
	bool TTN_Application::TTN_Input::GetKeyUp(TTN_KeyCode key)
	{
		//check if the key is currently down, if it isn't, and it was marked as being down, that means it's been release this frame
		if (!GetKey(key) && KeyWasPressedMap.at(key))
		{
			//return true to say it's been released
			KeyWasPressedMap.at(key) = false;
			return true;
		}
		//if not then the key is either still down, or was never put down in the first place, so just return false
		return false;
	}

	//returns the mouse position in screenspace
	glm::vec2 TTN_Application::TTN_Input::GetMousePosition()
	{
		//check if the mous is in the window
		if (inFrame)
		{
			//if it is get the position of it from glfw (if it's not this will just return the pos as of the last frame the mouse
			//was in the window)
			double tempX, tempY;
			glfwGetCursorPos(m_window, &tempX, &tempY);
			mousePos = glm::vec2(tempX, tempY);
		}
			
		//pass the mouse position to the user
		return mousePos;
	}

	//checks if a mouse button is being pressed
	bool TTN_Application::TTN_Input::GetMouseButton(TTN_MouseButton button)
	{
		//check if the button inputted currently exists in the map
		if (MouseWasPressedMap.find(button) == MouseWasPressedMap.end())
		{
			//if it doesn't, add it to the map (doing this here means we only add buttons to the map if the user acutally wants to use them) 
			MouseWasPressedMap[button] = false;
		}
		//check if the button has been pressed
		if (glfwGetMouseButton(m_window, static_cast<int>(button)))
		{
			//if it has, set it's place in the map to true
			MouseWasPressedMap.at(button) = true;
			//return true as the button is being pressed
			return true;
		}
		//if it hasn't been pressed, return false
		return false;
	}

	//checks if this is the first frame a mouse button is being pressed
	bool TTN_Application::TTN_Input::GetMouseButtonDown(TTN_MouseButton button)
	{
		//check first if the button exists in the map
		if (MouseWasPressedMap.find(button) == MouseWasPressedMap.end())
		{
			//if it doesn't, pass it onto the getMouseButton so it can create a place in the map, and check if it is being pressed
			if (GetMouseButton(button))
			{
				//if it returns true that means the button has been pressed this frame
				return true;
			}
		}
		//if it does exist in the map, check if the value in the map is false and if the button is being pressed
		else if (!MouseWasPressedMap.at(button) && GetMouseButton(button))
			//if it is, then this is the first frame where it's being pressed so return true
			return true;

		//if none of those are true, then either the button isn't being pressed, or this isn't the first frame it's being pressed
		//either way, return false
		return false;
	}

	bool TTN_Application::TTN_Input::GetMouseButtonUp(TTN_MouseButton button)
	{
		//check if the button is currently down, if it isn't, and it was marked as being down, that means it's been release this frame
		if (!GetMouseButton(button) && MouseWasPressedMap.at(button))
		{
			//return true to say it's been released
			MouseWasPressedMap.at(button) = false;
			return true;
		}
		//if not then the button is either still down, or was never put down in the first place, so just return false
		return false;
	}

	//gets from glfw wheter or not the mouse is in the frame, do not call as user
	void TTN_Application::TTN_Input::cursorEnterFrameCallback(GLFWwindow* window, int entered)
	{
		//set wheter or not the mouse is in the frame
		inFrame = entered;
	}
}