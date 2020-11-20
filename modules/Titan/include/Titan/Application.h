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
#include <unordered_map>


namespace Titan {
	//input enum class, converts titan keyboard input type to glfw keyboard input types
	enum TTN_KeyCode {
		Key_Unknown = GLFW_KEY_UNKNOWN,
		Space = GLFW_KEY_SPACE,
		Apostrophe = GLFW_KEY_APOSTROPHE,
		Comma = GLFW_KEY_COMMA,
		Minus = GLFW_KEY_MINUS,
		Period = GLFW_KEY_PERIOD,
		ForwardSlash = GLFW_KEY_SLASH,
		BackSlash = GLFW_KEY_BACKSLASH,
		Zero = GLFW_KEY_0,
		One = GLFW_KEY_1,
		Two = GLFW_KEY_2,
		Three = GLFW_KEY_3,
		Four = GLFW_KEY_4,
		Five = GLFW_KEY_5,
		Six = GLFW_KEY_6,
		Seven = GLFW_KEY_7,
		Eight = GLFW_KEY_8,
		Nine = GLFW_KEY_9,
		Semicolon = GLFW_KEY_SEMICOLON,
		Equals = GLFW_KEY_EQUAL,
		A = GLFW_KEY_A,
		B = GLFW_KEY_B,
		C = GLFW_KEY_C,
		D = GLFW_KEY_D,
		E = GLFW_KEY_E,
		F = GLFW_KEY_F,
		G = GLFW_KEY_G,
		H = GLFW_KEY_H,
		I = GLFW_KEY_I,
		J = GLFW_KEY_J,
		K = GLFW_KEY_K,
		L = GLFW_KEY_L,
		M = GLFW_KEY_M,
		N = GLFW_KEY_N,
		O = GLFW_KEY_O,
		P = GLFW_KEY_P,
		Q = GLFW_KEY_Q,
		R = GLFW_KEY_R,
		S = GLFW_KEY_S,
		T = GLFW_KEY_T,
		U = GLFW_KEY_U,
		V = GLFW_KEY_V,
		W = GLFW_KEY_W,
		X = GLFW_KEY_X,
		Y = GLFW_KEY_Y,
		Z = GLFW_KEY_Z,
		LeftSqaureBracket = GLFW_KEY_LEFT_BRACKET,
		RightSquareBracket = GLFW_KEY_RIGHT_BRACKET,
		Esc = GLFW_KEY_ESCAPE,
		Enter = GLFW_KEY_ENTER,
		Tab = GLFW_KEY_TAB,
		Backspace = GLFW_KEY_BACKSPACE,
		Insert = GLFW_KEY_INSERT,
		Delete = GLFW_KEY_DELETE,
		RightArrow = GLFW_KEY_RIGHT,
		LeftArrow = GLFW_KEY_LEFT,
		UpArrow = GLFW_KEY_UP,
		DownArrow = GLFW_KEY_DOWN,
		PageUp = GLFW_KEY_PAGE_UP,
		PageDown = GLFW_KEY_PAGE_DOWN,
		Home = GLFW_KEY_HOME,
		End = GLFW_KEY_END,
		CapsLock = GLFW_KEY_CAPS_LOCK,
		ScrollLock = GLFW_KEY_SCROLL_LOCK,
		NumLock = GLFW_KEY_NUM_LOCK,
		PrintScreen = GLFW_KEY_PRINT_SCREEN,
		Pause = GLFW_KEY_PAUSE,
		F1 = GLFW_KEY_F1,
		F2 = GLFW_KEY_F2,
		F3 = GLFW_KEY_F3,
		F4 = GLFW_KEY_F4,
		F5 = GLFW_KEY_F5,
		F6 = GLFW_KEY_F6,
		F7 = GLFW_KEY_F7,
		F8 = GLFW_KEY_F8,
		F9 = GLFW_KEY_F9,
		F10 = GLFW_KEY_F10,
		F11 = GLFW_KEY_F11,
		F12 = GLFW_KEY_F12,
		F13 = GLFW_KEY_F13,
		F14 = GLFW_KEY_F14,
		F15 = GLFW_KEY_F15,
		F16 = GLFW_KEY_F16,
		F17 = GLFW_KEY_F17,
		F18 = GLFW_KEY_F18,
		F19 = GLFW_KEY_F19,
		F20 = GLFW_KEY_F20,
		F21 = GLFW_KEY_F21,
		F22 = GLFW_KEY_F22,
		F23 = GLFW_KEY_F23,
		F24 = GLFW_KEY_F24,
		KeyPadZero = GLFW_KEY_KP_0,
		KeypadOne = GLFW_KEY_KP_1,
		KeyPadTwo = GLFW_KEY_KP_2,
		KeyPadThree = GLFW_KEY_KP_3,
		KeyPadFour = GLFW_KEY_KP_4,
		KeyPadFive = GLFW_KEY_KP_5,
		KeyPadSix = GLFW_KEY_KP_6,
		KeyPadSeven = GLFW_KEY_KP_7,
		KeyPadEight = GLFW_KEY_KP_8,
		KeyPadNine = GLFW_KEY_KP_9,
		KeyPadDecimal = GLFW_KEY_KP_DECIMAL,
		KeyPadDivide = GLFW_KEY_KP_DIVIDE,
		KeyPadTimes = GLFW_KEY_KP_MULTIPLY,
		KeyPadMinus = GLFW_KEY_KP_SUBTRACT,
		KeyPadPlus = GLFW_KEY_KP_ADD,
		KeyPadEnter = GLFW_KEY_KP_ENTER,
		KeyPadEquals = GLFW_KEY_KP_EQUAL,
		LeftShift = GLFW_KEY_LEFT_SHIFT,
		RightShift = GLFW_KEY_RIGHT_SHIFT,
		LeftControl = GLFW_KEY_LEFT_CONTROL,
		RightControl = GLFW_KEY_RIGHT_CONTROL,
		LeftAlt = GLFW_KEY_LEFT_ALT,
		RightAlt = GLFW_KEY_RIGHT_ALT,
		LeftSuper = GLFW_KEY_LEFT_SUPER,
		RightSuper = GLFW_KEY_RIGHT_SUPER,
		Menu = GLFW_KEY_MENU
	};

	//input enum class, converts titan mouse button input type to glfw mouse input types
	enum TTN_MouseButton {
		Left = GLFW_MOUSE_BUTTON_1,
		Right = GLFW_MOUSE_BUTTON_2,
		Middle = GLFW_MOUSE_BUTTON_3,
		Fourth = GLFW_MOUSE_BUTTON_4,
		Fifth = GLFW_MOUSE_BUTTON_5,
		Sixth = GLFW_MOUSE_BUTTON_6,
		Seventh = GLFW_MOUSE_BUTTON_7,
		Eighth = GLFW_MOUSE_BUTTON_8
	};

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
		static std::vector<TTN_Scene*> scenes;
		static GLFWwindow* m_window;

	protected:
		//default constructor, just creates an empty aplication project
		TTN_Application() = default;

		
		static float m_dt;
		static float m_previousFrameTime;

	public:


		//input helper class
		class TTN_Input {
		public:
			//checks if a key is being pressed this frame
			static bool GetKey(TTN_KeyCode key);

			//checks if a key button has been pressed for it's first frame this frame
			static bool GetKeyDown(TTN_KeyCode key);

			//checks if a key button has been released this frame
			static bool GetKeyUp(TTN_KeyCode key);

			//returns the mouse position in screenspace
			static glm::vec2 GetMousePosition();

			//checks if a mouse button is being pressed this frame
			static bool GetMouseButton(TTN_MouseButton button);

			//checks if a mouse button has been pressed for it's first frame this frame
			static bool GetMouseButtonDown(TTN_MouseButton button);

			//checks if a mouse button has been released this frame
			static bool GetMouseButtonUp(TTN_MouseButton button);

			//hides or unhides the cursor based on an inputed bool
			static void SetCursorHidden(bool hidden);

			//gets from glfw wheter or not the mouse is in frame, do not call as user
			static void cursorEnterFrameCallback(GLFWwindow *window, int entered);

		protected:
			//map of the booleans for if a key is being pressed
			static std::unordered_map<TTN_KeyCode, bool> KeyWasPressedMap;
			//map of booleans for if a mouse button is being pressed
			static std::unordered_map<TTN_MouseButton, bool> MouseWasPressedMap;

			//position of the mouse
			static glm::vec2 mousePos;
			//boolean for if the mouse in currently in the window
			static bool inFrame;
		};
	};
}
