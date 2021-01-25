//Titan Engine, by Atlas X Games 
// MAnimator.cpp - source file for the class that representing a component class to manage morph target animations

//precompile header
#include "Titan/ttn_pch.h"
//include the header 
#include "Titan/MAnimator.h"

namespace Titan {
	//default constructor
	TTN_MorphAnimator::TTN_MorphAnimator()
		: m_CurrentAnim(0)
	{
		m_Anims = std::vector<TTN_MorphAnimation>();
	}

	//constructor that takes in data
	TTN_MorphAnimator::TTN_MorphAnimator(std::vector<TTN_MorphAnimation> anims, int activeAnim)
		: m_CurrentAnim(activeAnim), m_Anims(anims)
	{
	}

	//Adds a morph target animation to this animator
	void TTN_MorphAnimator::AddAnim(TTN_MorphAnimation anim)
	{
		m_Anims.push_back(anim);
	}

	//sets which animtaion is currently active
	void TTN_MorphAnimator::SetActiveAnim(int index)
	{
		if (index < m_Anims.size() && index > -1)
			m_CurrentAnim = index;
		else
			LOG_ERROR("You cannot set an animation that doesn't exist as the active animation");
	}
}