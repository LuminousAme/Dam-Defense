//Titan Engine, by Atlas X Games 
// Application.cpp - source file for the class that runs the program, creating the window, etc.

//include the precompile header, this file uses stdio.h
#include "Titan/ttn_pch.h"
//include the header 
#include "Titan/Application.h"
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.cpp"
#include "imgui_impl_opengl3.cpp"

namespace Titan {
	//set the base values for the member variables
	GLFWwindow* TTN_Application::m_window = nullptr;
	float TTN_Application::m_dt = 0.0f;
	float TTN_Application::m_previousFrameTime = 0.0f;
	std::vector<TTN_Scene*> TTN_Application::scenes = std::vector<TTN_Scene*>();
	std::unordered_map<TTN_KeyCode, bool> TTN_Application::TTN_Input::KeyWasPressedMap;
	std::unordered_map<TTN_KeyCode, bool> TTN_Application::TTN_Input::KeyPressed;
	std::unordered_map<TTN_KeyCode, bool> TTN_Application::TTN_Input::KeyHandled;
	std::unordered_map<TTN_MouseButton, bool> TTN_Application::TTN_Input::MouseWasPressedMap;
	std::unordered_map<TTN_MouseButton, bool> TTN_Application::TTN_Input::MousePressed;
	std::unordered_map<TTN_MouseButton, bool> TTN_Application::TTN_Input::MouseHandled;
	glm::vec2 TTN_Application::TTN_Input::mousePos = glm::vec2(0.0f);
	bool TTN_Application::TTN_Input::inFrame = false;
	std::vector<std::function<void()>> imGuiCallbacks; 

	//function to initialize a new window 
	void TTN_Application::Init(const std::string name, int width, int height, bool fullScreen)
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
		if(!fullScreen) m_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
		else m_window = glfwCreateWindow(width, height, name.c_str(), glfwGetPrimaryMonitor(), nullptr);

		//set the window we want to draw on to the window that was just created
		glfwMakeContextCurrent(m_window);

		//send the window to backend so other parts of titan can access the screensize
		TTN_Backend::setWindow(m_window);

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

		//enable the depth function for skyboxes
		glDepthFunc(GL_LEQUAL);

		//enable blend function to allow for transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//enable clipping planes
		glEnable(GL_CLIP_DISTANCE0);

		//set up the shader program for the particle system
		TTN_ParticleSystem::InitParticleShader();

		//set up the shader program for shadow mapping
		TTN_Renderer::InitShadowRendering();

		//set up the shader and vaos for the sprite rendering system
		TTN_Renderer2D::InitRenderer2D();

		//set up the scene buffers
		TTN_Scene::InitBuffers();

		//setup the materials
		TTN_Material::Init();

		//set up the audio engine
		m_soundEngine.Init();
		
		//Set the background colour for our scene to the base black
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	}

	//function to check if the window is being closed
	bool TTN_Application::GetIsClosing()
	{
		//check if quit() has been called, if it has, return that the application is closing
		if (m_hasQuit)
			return m_hasQuit;

		//otherwise have glfw check if the user has tried to close the window, return what it says
		return glfwWindowShouldClose(m_window);
	}

	//function that cleans things up when the window closes so there are no memory leaks and everything goes cleanly 
	void TTN_Application::Closing()
	{
		//clean up imgui
		CleanImgui();

		//shutdown the audio engine
		m_soundEngine.Shutdown();

		//have glfw destroy the window 
		glfwDestroyWindow(m_window);

		//close glfw
		glfwTerminate();

		//delete scene pointers
		for (auto x : scenes)
			delete x;
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
		glClearDepth(1.0f);
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
		if (!m_hasQuit) {
			//start a new frame 
			TTN_Application::NewFrameStart();

			//start ImGui
			StartImgui();

			//check for events from glfw 
			glfwPollEvents();

			//update the asset system
			TTN_AssetSystem::Update();

			//go through each scene 
			for (int i = 0; i < TTN_Application::scenes.size(); i++) {
				//and check if they should be rendered
				if (TTN_Application::scenes[i]->GetShouldRender()) {
					//if they should, then check input, update, and render them 
					TTN_Application::scenes[i]->KeyDownChecks();
					TTN_Application::scenes[i]->KeyChecks();
					TTN_Application::scenes[i]->KeyUpChecks();

					TTN_Application::scenes[i]->MouseButtonDownChecks();
					TTN_Application::scenes[i]->MouseButtonChecks();
					TTN_Application::scenes[i]->MouseButtonUpChecks();

					TTN_Application::scenes[i]->Update(m_dt);
					TTN_Application::scenes[i]->Render();
					TTN_Application::scenes[i]->PostRender();
				}
			}

			//reset the keys so they work properly on the next frame
			TTN_Application::TTN_Input::ResetKeys();
			//reset the mouse buttons so they work properly on the next frame
			TTN_Application::TTN_Input::ResetMouseButtons();

			//update the sound engine
			m_soundEngine.Update();

			//now all the scenes that should be rendered (current gameplay scene, ui, etc.) will be rendered
			//while anything that doesn't need to be rendered (such as a prefabs scene) will not 

			//end Imgui, rendering it
			EndImgui();

			//set the last effect to nullpointer so it's set up correctly for the next frame
			TTN_Backend::SetLastEffect(nullptr);

			//swap the buffers so all the drawings that the scenes just did are acutally visible 
			glfwSwapBuffers(m_window);
		}
	}

	//quits the application
	void TTN_Application::Quit()
	{
		//check if quit hasn't already been called
		if (!m_hasQuit) {
			//if hasn't set the quit flag to true and call the closing function
			m_hasQuit = true;
		}
	}

#pragma region IMGUI STUFF

	//init's imgui stuff
	void TTN_Application::InitImgui()
	{
		LOG_INFO(glGetString(GL_VERSION));
		//printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
		// Creates a new ImGUI context
		ImGui::CreateContext();
		// Gets our ImGUI input/output
		ImGuiIO& io = ImGui::GetIO();
		// Enable keyboard navigation
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		// Allow docking to our window
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		// Allow multiple viewports (so we can drag ImGui off our window)
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		// Allow our viewports to use transparent backbuffers
		io.ConfigFlags |= ImGuiConfigFlags_TransparentBackbuffers;

		// Set up the ImGui implementation for OpenGL
		ImGui_ImplGlfw_InitForOpenGL(m_window, true);
		ImGui_ImplOpenGL3_Init("#version 420");

		// Dark mode 
		ImGui::StyleColorsDark();

		// Get our imgui style
		ImGuiStyle& style = ImGui::GetStyle();
		//style.Alpha = 1.0f;
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 0.8f;

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 0.8f;
		}
		//	std::cout << "Hello" << std::endl;
	}

	//start
	void TTN_Application::StartImgui() {
		// Implementation new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		// ImGui context new frame
		ImGui::NewFrame();
	}

	void TTN_Application::EndImgui() {
		// Make sure ImGui knows how big our window is
		ImGuiIO& io = ImGui::GetIO();
		int width, height;
		glfwGetWindowSize(m_window, &width, &height);
		io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));

		// Render all of our ImGui elements
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// If we have multiple viewports enabled (can drag into a new window)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			// Update the windows that ImGui is using
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			// Restore our gl context
			glfwMakeContextCurrent(m_window);
		}
	}

	//called after program loop to delete some context window stuff
	void TTN_Application::CleanImgui() {
		// Cleanup the ImGui implementation
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		// Destroy our ImGui context
		ImGui::DestroyContext();
	}

#pragma endregion

	//checks if a key is being pressed
	bool TTN_Application::TTN_Input::GetKey(TTN_KeyCode key)
	{
		//check if the keyinputted currently exists in the map
		if (KeyWasPressedMap.find(key) == KeyWasPressedMap.end())
		{
			//if it doesn't, add it to the map (doing this here means we only add keys to the map if the user acutally wants to use them) 
			KeyWasPressedMap[key] = false;
			KeyHandled[key] = false;
			KeyPressed[key] = false;
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
				//if it sets the flag and return true that means the key has been pressed this frame
				KeyHandled.at(key) = true;
				KeyPressed.at(key) = true;
				return true;
			}
		}
		//if it does exist in the map, check if it was already handled this frame
		else if (KeyHandled.at(key)) {
			return true;
		}
		//if it does exist in the map and was not handled yet this frame, check if the value in the map is false and if the key is being pressed
		else if (GetKey(key) && !KeyPressed.at(key)) {
			//if it is, then this is the first frame where it's being pressed so set the flags and return true
			KeyHandled.at(key) = true;
			KeyPressed.at(key) = true;

			return true;
		}
		//if it's not being pressed make sure the flag reflects that
		else if (!GetKey(key)) {
			KeyPressed.at(key) = false;
		}

		//and return false as it's not being pressed
		return false;
	}

	//checks if a key was pressed down, but has now been released
	bool TTN_Application::TTN_Input::GetKeyUp(TTN_KeyCode key)
	{
		//check if the key is currently down, if it isn't, and it was marked as being down, that means it's been release this frame
		if (!GetKey(key) && KeyWasPressedMap.at(key))
		{
			//return true to say it's been released
			return true;
		}
		//if not then the key is either still down, or was never put down in the first place, so just return false
		return false;
	}

	//call once a frame to make the input system work
	void TTN_Application::TTN_Input::ResetKeys()
	{
		//reset each key
		for (auto& it : KeyWasPressedMap) {
			//if it's not currently being pressed, we need to reset the was pressed flag
			if (!glfwGetKey(m_window, static_cast<int>(it.first)))
				it.second = false;
		}
		for (auto& it : KeyHandled) {
			it.second = false;
		}
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
			MouseHandled[button] = false;
			MousePressed[button] = false;
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
				//if it sets the flag and return true that means the key has been pressed this frame
				MouseHandled.at(button) = true;
				MousePressed.at(button) = true;
				return true;
			}
		}
		//if it does exist in the map, check if it was handleded already this frame
		else if (MouseHandled.at(button)) {
			//if it was, then return true
			return true;
		}
		//if it does exist in the map and was not yet handleded in this frame, check if the value in the map is false and if the button is being pressed
		else if (GetMouseButton(button) && !MousePressed.at(button)) {
			//if it is, then this is the first frame where it's being pressed so set the flags and return true
			MouseHandled.at(button) = true;
			MousePressed.at(button) = true;

			return true;
		}
		//if it's not being pressed make sure the flag reflects that
		else if (!GetMouseButton(button)) {
			MousePressed.at(button) = false;
		}

		//and return false as it's not being pressed
		return false;
	}

	//checks if a mouse button has been pressed but has now been released
	bool TTN_Application::TTN_Input::GetMouseButtonUp(TTN_MouseButton button)
	{
		//check if the button is currently down, if it isn't, and it was marked as being down, that means it's been release this frame
		if (!GetMouseButton(button) && MouseWasPressedMap.at(button))
		{
			//return true to say it's been released
			return true;
		}
		//if not then the button is either still down, or was never put down in the first place, so just return false
		return false;
	}

	//call once a frame to make the input system work
	void TTN_Application::TTN_Input::ResetMouseButtons()
	{
		//reset each mouse button
		for (auto& it : MouseWasPressedMap) {
			//if it's not currently being pressed, we need to reset the was pressed flag
			if (!glfwGetMouseButton(m_window, static_cast<int>(it.first)))
				it.second = false;
		}
		for (auto& it : MouseHandled) {
			it.second = false;
		}
	}

	//sets wheter or not the cursor is visible
	void TTN_Application::TTN_Input::SetCursorHidden(bool hidden)
	{
		//if they want to hide the cursor, tell glfw to make it invisible
		if(!hidden) glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		//otherwise make it visible as normal
		else  glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	//sets wheter or not the cursor is locked
	void TTN_Application::TTN_Input::SetCursorLocked(bool locked)
	{
		//if they want to lock the cursor, lock it
		if (locked) glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	//gets from glfw wheter or not the mouse is in the frame, do not call as user
	void TTN_Application::TTN_Input::cursorEnterFrameCallback(GLFWwindow* window, int entered)
	{
		//set wheter or not the mouse is in the frame
		inFrame = entered;
	}

	//gets the window width from glfw
	glm::ivec2 TTN_Application::TTN_Input::GetWidth()
	{
		GLFWvidmode query;

		int width, height;
		glfwGetWindowSize(m_window, &width, &height);
		return glm::ivec2(width, height);
	}
 
}