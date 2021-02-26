//Titan Engine, by Atlas X Games 
// 2DAnimation.cpp - source file for the class that representing 2D animations

//precompile header
#include "Titan/ttn_pch.h"
//include the header 
#include "Titan/Systems/2DAnimation.h"

namespace Titan {
	//defualt constructor
	TTN_2DAnimation::TTN_2DAnimation() {
		m_timer = 0.0f;
		m_frames = std::vector<TTN_Texture2D::st2dptr>();
		m_frameTimes = std::vector<float>();
		m_totalTime = 0.0f;
		m_currentIndex = 0;
		m_shouldLoop = true;
		m_paused = false;
		m_PlaybackSpeedFactor = 1.0f;
	}

	//constructor with data
	TTN_2DAnimation::TTN_2DAnimation(std::vector<TTN_Texture2D::st2dptr> frames, std::vector<float> frameTimes, bool shouldLoop, float playbackFactor) {
		m_timer = 0.0f;
		SetFrames(frames, frameTimes);
		m_shouldLoop = shouldLoop;
		m_paused = false;
		m_PlaybackSpeedFactor = playbackFactor;
	}
	
	//update every frame
	void TTN_2DAnimation::Update(float deltaTime) {
		//if it is not paused and there is acutally lenght to the animation
		if (!m_paused) {
			if (m_totalTime != 0) {
				//if theres's only one frame just set it to that frame
				if (m_frames.size() == 1) {
					m_currentIndex = 1;
					m_isDone = true;
					return;
				}

				//if there's 2 or more frames, travel through the animation
				if (m_frames.size() >= 2) {
					m_isDone = false;
					m_timer += deltaTime * m_PlaybackSpeedFactor;

					//when a frame finishes
					if (m_timer > m_frameTimes[m_currentIndex]) {
						//move to the next frame
						m_currentIndex++;

						//if it's on the last for current index
						if (m_currentIndex > m_frames.size() - 1) {
							//mark the animation as done
							m_isDone = true;
							//if it should loop, reset it to zero
							if (m_shouldLoop)
								m_currentIndex = 0;
							//otherwise keep it on the last frame1
							else
								m_currentIndex = m_frames.size() - 1;
						}
					}
				}

				//get the remainder of the timer with the time the keyframe ends
				m_timer = fmod(m_timer, m_frameTimes[m_currentIndex]);
			}
		}
	}

	//restart at the end of the animation
	void TTN_2DAnimation::Restart() {
		m_currentIndex = 0;
		m_timer = 0.0f;
		m_isDone = false;
	}

	//set frames for the animation
	void TTN_2DAnimation::SetFrames(std::vector<TTN_Texture2D::st2dptr> frames, std::vector<float> frameTimes)
	{
		//copy both lists
		m_frames = frames;
		m_frameTimes = frameTimes;

		//log an error if there are no keyframes
		if (m_frames.size() < 1) {
			LOG_ERROR("You cannot have a 2D animation with no keyframes");
		}

		//log a warning if theres a size mismatch
		if (m_frames.size() != m_frameTimes.size()) {
			LOG_WARN("2D Animation should have an equal number of keyframes and keyframe lenghts");
		}

		//set the total time
		m_totalTime = m_frameTimes[m_frameTimes.size() - 1];

		//restart the animation
		Restart();
	}
}