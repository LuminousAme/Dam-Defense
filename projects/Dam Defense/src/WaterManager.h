//Dam Defense by Atlas X Games
//WaterManager.h, the header for the class that manages the water effect
#pragma once

//import titan features
#include "Titan/Application.h"
#include "Titan/Utilities/Interpolation.h"
#include "Titan/Utilities/Random.h"

using namespace Titan;

class WaterManager {
public:
	static void Init();

	static void MakeAWave(int index);

	static void Update(float deltaTime);

	static void SetSpeed(float speed) { m_speed = speed; }
	static float GetSpeed() { return m_speed; }

	static void SetMedianWaveLenght(float medianWaveLenght) { m_medianWaveLenght = medianWaveLenght; }
	static float GetMedianWaveLenght() { return m_medianWaveLenght; }

	static void SetMedianAmplitude(float medianAmplitude) { m_medianAmplitude = medianAmplitude; }
	static float GetMedianAmplitude() { return m_medianAmplitude; }

	static void SetSteepness(float steepness) { m_steepness = steepness; }
	static float GetSteepness() { return m_steepness; }

	static void SetBaseDirection(glm::vec3 direction) { m_basedirection = direction; }
	static glm::vec3 GetBaseDirection() { return m_basedirection; }

	static void SetAcceptableAngle(float acceptableAngle) { m_angle = acceptableAngle; }
	static float GetAcceptableAngle() { return m_angle; }

	static void SetGravitationalConstant(float grav) { m_gravationalConstant = grav; }
	static float GetGravitationalConstant() { return m_gravationalConstant; }

	static void SetLifeTime(float lifeTime) { m_lifeTime = lifeTime; }
	static float GetLifeTime() { return m_lifeTime; }

	static void SetNumberOfWaves(int numberOfWaves) { m_numberOfWaves = numberOfWaves; }
	static float GetNumberOfWaves() { return m_numberOfWaves; }

	static void SetTime(float time) { m_time = time; }
	static float GetTime() { return m_time; }

	static void SetVoronoiSpeed(float speed) { m_voronoiSpeed = speed; }
	static float GetVoronoiSpeed() { return m_voronoiSpeed; }

	static void SetVoronoiGridFidelity(float fidelity) { m_voronoiGridFidelty = fidelity; }
	static float GetVoronoiGridFidelity() { return m_voronoiGridFidelty; }

	static void BindVoronoiAsColor(int index) { m_voronoiBuffer->BindColorAsTexture(0, index); }
	static void UnbindVoronoi(int index) { m_voronoiBuffer->UnbindTexture(index); }

private:
	//general variables
	inline static float m_speed = 0.5f;
	inline static float m_medianWaveLenght = 0.1f;
	inline static float m_medianAmplitude = 0.0015f;
	inline static float m_steepness = 0.45f;
	inline static glm::vec3 m_basedirection = glm::vec3(0.0f, 0.0f, -1.0f);
	inline static float m_angle = 10.0f;
	inline static float m_gravationalConstant = 0.98f;
	inline static float m_lifeTime = 60.0f;
	inline static int m_numberOfWaves = 4;

	//time tracker
	inline static float m_time = 0.0f;

public:
	//per wave variables
	inline static std::vector<float> m_waveFrequency = std::vector<float>();
	inline static std::vector<glm::vec3> m_waveDirection = std::vector<glm::vec3>();;
	inline static std::vector<float> m_waveAcutalAmplitude = std::vector<float>();
	inline static std::vector<float> m_waveTargetAmplitude = std::vector<float>();
	inline static std::vector<float> m_waveTimeAlive = std::vector<float>();

private:
	//variables for the voronoi effect
	inline static TTN_Framebuffer::sfboptr m_voronoiBuffer = nullptr;
	inline static TTN_Shader::sshptr m_voronoiShader = nullptr;

	inline static float m_voronoiSpeed = 0.0000045f;
	inline static float m_voronoiGridFidelty = 16.0f;
};

inline float waterLifeInterpolationParameter(float t) {
	float value = t;
	if (t <= 0.25f) {
		value = TTN_Interpolation::ReMap(0.0f, 0.25f, 0.5f, 1.0f, t);
	}
	else if (t > 0.25f && t <= 0.75f) {
		value = 1.0f;
	}
	else {
		value = TTN_Interpolation::ReMap(0.75f, 1.0f, 1.0f, 0.5f, t);
	}

	value = glm::clamp(value, 0.0f, 1.0f);

	return value;
}