//Titan Engine, by Atlas X Games 
// 2DAnimator.h - header for the class that representing a component class to manage 2D animations
#pragma once

//include the 2D animations class
#include "Titan/Systems/2DAnimation.h"

namespace Titan {
	//class to manage 2D animations
	class TTN_2DAnimator {
	public:
		//Default constructor
		TTN_2DAnimator();

		//construcot that takes data
		TTN_2DAnimator(std::vector<TTN_2DAnimation> anims, int activeAnim);

		//default destructor
		~TTN_2DAnimator() = default;

		//adds an animation
		void AddAnim(TTN_2DAnimation anim) { m_Anims.push_back(anim); }

		//sets the current animation
		void SetActiveAnim(int index);

		//gets the current animation
		TTN_2DAnimation& GetActiveAnimRef() { return m_Anims[m_CurrentAnim]; }
		//Gets the animation at a given index
		TTN_2DAnimation& GetAnimRefAtIndex(int index) { return m_Anims[index]; }
		//gets the index of the current animation
		int GetActiveAnim() { return m_CurrentAnim; }

	private:
		std::vector<TTN_2DAnimation> m_Anims;
		int m_CurrentAnim;
	};
}