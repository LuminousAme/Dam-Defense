//Titan Engine, by Atlas X Games 
// Backend.h - header for the class that stores backend data, mostly exists to be able to pass data between application and scene
//might move a bunch of application's stuff here later
#pragma once

//include precompile header
#include "Titan/ttn_pch.h"

//include glfw
#include <GLFW/glfw3.h>
//include titan features the entire application needs to be able to access
#include "Titan/Graphics/Post/PostEffect.h"

namespace Titan {
	//backend static class
	class TTN_Backend {
	public:
		//sets the pointer to the window 
		static void setWindow(GLFWwindow* window) { m_window = window; }
		//gets the window size
		static glm::ivec2 GetWindowSize();

		//sets the last effect
		static void SetLastEffect(TTN_PostEffect::spostptr lastEffect) { m_lastEffect = lastEffect; }
		//gets the last effect
		static TTN_PostEffect::spostptr GetLastEffect() { return m_lastEffect; }

	private:
		//pointer to the window
		inline static GLFWwindow* m_window = nullptr;
		//pointer to the last buffer applied
		inline static TTN_PostEffect::spostptr m_lastEffect = nullptr;
	};
}