//Titan Engine, by Atlas X Games 
// MAnimation.h - header for the class that representing a component class to manage morph target animations
#pragma once

//include the morph target animations class
#include "MAnimation.h"

namespace Titan {
	//class to manage morph target animation
	class TTN_MorphAnimator {
	public:
		//default constructor
		TTN_MorphAnimator();

		//constructor that takes data
		TTN_MorphAnimator(std::vector<TTN_MorphAnimation> anims, int activeAnim);

		//default destructor 
		~TTN_MorphAnimator() = default;

		//add an aniatmion
		void AddAnim(TTN_MorphAnimation anim);

		//sets the current animation
		void SetActiveAnim(int index);

		//gets the current animation
		TTN_MorphAnimation& getActiveAnimRef() { return m_Anims[m_CurrentAnim]; }
		//gets the animation at a given index
		TTN_MorphAnimation& getAnimRefAtIndex(int index) { return m_Anims[m_CurrentAnim]; }
		//gets the index of the current animation
		int getActiveAnim() { return m_CurrentAnim; }

	private:
		std::vector<TTN_MorphAnimation> m_Anims;
		int m_CurrentAnim;
	};
}