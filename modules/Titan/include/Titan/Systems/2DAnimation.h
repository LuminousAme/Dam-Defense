//Titan Engine, by Atlas X Games 
// 2DAnimation.h - header for the class that representing 2D animations
#pragma once

//precompile header, this file uses GLM/glm.hpp, vector, and Logging.h
#include "Titan/ttn_pch.h"
//include 2D textures so it can access them
#include "Titan/Graphics/Texture2D.h"

namespace Titan {
	class TTN_2DAnimation {
	public:
		//default constructor
		TTN_2DAnimation();

		//constructor that takes in data
		TTN_2DAnimation(std::vector<TTN_Texture2D::st2dptr> frames, std::vector<float> frameTimes, bool shouldLoop = true, float playbackFactor = 1.0f);

		//default destructor
		~TTN_2DAnimation() = default;

		//update the animation each frame
		void Update(float deltaTime);

		//restart the animation at the end 
		void Restart();

		//setters
		void SetFrames(std::vector<TTN_Texture2D::st2dptr> frames, std::vector<float> frameTimes);
		void SetPaused(bool paused) { m_paused = paused; }
		void SetShouldLoop(bool shouldLoop) { m_shouldLoop = shouldLoop; }
		void SetPlaybackSpeedFactor(float playbackFactor) { m_PlaybackSpeedFactor = playbackFactor; }

		//getters
		std::vector<TTN_Texture2D::st2dptr> GetFrames() { return m_frames; }
		std::vector<float> GetFrameLenghts() { return m_frameTimes; }
		TTN_Texture2D::st2dptr GetCurrentFrame() { return m_frames[m_currentIndex]; }
		bool GetPaused() { return m_paused; }
		bool GetShouldLoop() { return m_shouldLoop; }
		float GetPlaybackSpeedFactor() { return m_PlaybackSpeedFactor; }
		bool GetIsDone() { return m_isDone; }

	private:
		float m_timer; //internal controller, the current time through the animation
		std::vector<TTN_Texture2D::st2dptr> m_frames; //external controller, the textures that should be used as frames
		std::vector<float> m_frameTimes; //external controller, how at what time should each frame play
		float m_totalTime; //internal controller, the lenght of the entire animation, sum of all the elements in m_frameLenghts
		int m_currentIndex; //interal controller, which frame it's currently on (index of m_frames)
		bool m_shouldLoop; //if the animation should loop
		bool m_paused; //if the animation is currently paused
		float m_PlaybackSpeedFactor; //the speed at which the animation plays back
		bool m_isDone; //boolean to mark if the animation has ended (only lasts a frame on looping animations)
	};
}