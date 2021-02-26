//Titan Engine, by Atlas X Games 
// 2DAnimatior.cpp - source file for the class that manages 2D animations

//precompile header
#include "Titan/ttn_pch.h"
//include the header 
#include "Titan/Systems/2DAnimator.h"

namespace Titan {
	//default constructor
	TTN_2DAnimator::TTN_2DAnimator()
		: m_CurrentAnim(0)
	{
		m_Anims = std::vector<TTN_2DAnimation>();
	}

	TTN_2DAnimator::TTN_2DAnimator(std::vector<TTN_2DAnimation> anims, int activeAnim)
		: m_Anims(anims)
	{
		SetActiveAnim(activeAnim);
	}

	//sets the current animation
	void TTN_2DAnimator::SetActiveAnim(int index)
	{
		if (index < m_Anims.size() && index > -1)
			m_CurrentAnim = index;
		else
			LOG_ERROR("You cannot set an animation that doesn't exist as the active animation");
	}
}