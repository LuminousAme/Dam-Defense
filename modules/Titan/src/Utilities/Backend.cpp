#include "Titan/ttn_pch.h"
#include "Titan/Utilities/Backend.h"

namespace Titan {
	glm::ivec2 Titan::TTN_Backend::GetWindowSize()
	{
		int width, height;
		glfwGetWindowSize(m_window, &width, &height);
		return glm::ivec2(width, height);
	}


}
