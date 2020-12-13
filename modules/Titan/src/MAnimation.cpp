//Titan Engine, by Atlas X Games 
// MAnimation.cpp - source file for the class that representing morph target animations

//precompile header
#include "Titan/ttn_pch.h"
//include the header 
#include "Titan/MAnimation.h"

namespace Titan {
	//defualt constructor
	TTN_MorphAnimation::TTN_MorphAnimation()
	{
		m_timer = 0.0f;
		m_frameIndices = std::vector<int>();
		m_frameTimes = std::vector<float>();
		m_totalTime = 0.0f;
		m_currentIndex = 0;
		m_nextIndex = 0;
		m_shouldLoop = true;
		m_paused = false;
		m_PlaybackSpeedFactor = 1.0f;
		m_interpolationParameter = 0.0f;
	}
	
	//constructor with data
	TTN_MorphAnimation::TTN_MorphAnimation(std::vector<int> frameIndices, std::vector<float> frameTimes, bool shouldLoop, float playbackFactor)
	{
		m_timer = 0.0f;
		setFrames(frameIndices, frameTimes);
		m_shouldLoop = shouldLoop;
		m_paused = false;
		m_PlaybackSpeedFactor = playbackFactor;
		m_interpolationParameter = 1.0f;
	}

	//updates the animation each frame it is playing
	void TTN_MorphAnimation::Update(float deltaTime)
	{
		//if it's not paused and there is acutally lenght to the animation
		if (!m_paused) {
			if (m_totalTime != 0) {
				//if there's no frames
				if (m_frameIndices.size() == 0) {
					//just return as there is no data
					return;
				}
				//if there's only 1 frame just set it to that frame
				if (m_frameIndices.size() == 1) {
					m_currentIndex = 0;
					m_nextIndex = 0;
					m_interpolationParameter = 0;
					m_isDone = true;
					return;
				}
				//if there's 2 or more frames, travel through the animation
				if (m_frameIndices.size() >= 2) {
					m_isDone = false;
					m_timer += deltaTime * m_PlaybackSpeedFactor;

					//when a frame finishes
					if (m_timer > m_frameTimes[m_currentIndex]) {
						//move to the next frame
						m_currentIndex++;

						//if it's on the last for current index
						if (m_currentIndex > m_frameIndices.size() - 1) {
							//mark the animation as done
							m_isDone = true;
							//if it should loop, reset it to zero
							if (m_shouldLoop)
								m_currentIndex = 0;
							//otherwise keep it on the last frame1
							else
								m_currentIndex = m_frameIndices.size() - 1;
						}

						m_nextIndex = m_currentIndex + 1;

						//if it's on the last for next index
						if (m_nextIndex > m_frameIndices.size() - 1) {
							//if it should loop, reset it to zero
							if (m_shouldLoop)
								m_nextIndex = 0;
							//otherwise keep it on the last frame1
							else
								m_nextIndex = m_frameIndices.size() - 1;
						}
					}

					//get the remainder of the timer with the time the keyframe ends
					m_timer = fmod(m_timer, m_frameTimes[m_currentIndex]);

					m_interpolationParameter = m_timer / m_frameTimes[m_currentIndex];
				}
			}
			else {
				//if the total time is zero just make t equal to zero
				m_interpolationParameter = 0;
			}
		
		}
	}

	//restart the animation
	void TTN_MorphAnimation::Restart()
	{
		m_currentIndex = 0;
		m_nextIndex = 1;
		m_interpolationParameter = 0;
		m_timer = 0;
		m_isDone = false;
	}

	//sets the frames for the animation
	void TTN_MorphAnimation::setFrames(std::vector<int> frameIndices, std::vector<float> frameTimes)
	{
		//copy both lists
		m_frameIndices = frameIndices;
		m_frameTimes = frameTimes;

		//log a warning if theres a size mismatch
		if (m_frameIndices.size() != m_frameTimes.size()) {
			LOG_WARN("Animation should have an equal number of keyframes and keyframe lenghts");
		}
		
		//set the total tile
		m_totalTime = m_frameTimes[m_frameTimes.size() - 1];

		//restart the animation
		Restart();
	}

	//sets wheter or not the animation is paused or playing
	void TTN_MorphAnimation::setPaused(bool paused)
	{
		m_paused = paused;
	}

	//sets wheter or not the animation should loop
	void TTN_MorphAnimation::setShouldLoop(bool shouldLoop)
	{
		m_shouldLoop = shouldLoop;
	}

	//sets a multiplier for how fast the animation should be playing back
	void TTN_MorphAnimation::SetPlaybackSpeedFactor(float playbackFactor)
	{
		m_PlaybackSpeedFactor = playbackFactor;
	}
}