//Titan Engine, by Atlas X Games 
// MAnimation.h - header for the class that representing morph target animations
#pragma once

//precompile header, this file uses GLM/glm.hpp, vector, and Logging.h
#include "ttn_pch.h"

namespace Titan {
	class TTN_MorphAnimation {
	public:
		//default constructor
		TTN_MorphAnimation();

		//constructor that takes data
		TTN_MorphAnimation(std::vector<int> frameIndices, std::vector<float> frameTimes, bool shouldLoop = true, float playbackFactor = 1.0f);

		//default destructor
		~TTN_MorphAnimation() = default;

		//update the animation each frame
		void Update(float deltaTime);

		//restart the animation at the end
		void Restart();

		//setters
		void setFrames(std::vector<int> frameIndices, std::vector<float> frameTimes);
		void setPaused(bool paused);
		void setShouldLoop(bool shouldLoop);
		void SetPlaybackSpeedFactor(float playbackFactor);

		//gettters
		std::vector<int> getFrameIndices() { return m_frameIndices; }
		std::vector<float> getFrameLenghts() { return m_frameTimes; }
		bool getPaused() { return m_paused; }
		bool getShouldLoop() { return m_shouldLoop; }
		float getPlaybackSpeedFactor() { return m_PlaybackSpeedFactor; }
		int getCurrentMeshIndex() { return m_frameIndices[m_currentIndex]; }
		int getNextMeshIndex() { return m_frameIndices[m_nextIndex]; }
		float getInterpolationParameter() { return m_interpolationParameter; }
		bool getIsDone() { return m_isDone; }

	private:
		float m_timer; //internal controller, the current time through the animation
		std::vector<int> m_frameIndices; //external controller, the mesh indices that should play
		std::vector<float> m_frameTimes; //external controller, how at what time should each frame play
		float m_totalTime; //internal controller, the lenght of the entire animation, sum of all the elements in m_frameLenghts
		int m_currentIndex; //interal controller, which frame it's currently on (index of m_frameIndices)
		int m_nextIndex; //internal controller, which frame it's lerping to (index of m_frameIndices)
		bool m_shouldLoop; //if the animation should loop
		bool m_paused; //if the animation is currently paused
		float m_PlaybackSpeedFactor; //the speed at which the animation plays back
		float m_interpolationParameter; //t for lerp
		bool m_isDone; //boolean to mark if the animation has ended (only lasts a frame on looping animations)
	};
}
