//Dam Defense by Atlas X Games
//ColorBlind.h, the header file for the scene class that overlays the colour blind filters
#pragma once

//include required features from titan
#include "Titan/Application.h"

using namespace Titan;

class ColorBlindFilter : public TTN_Scene {
public:
	//default constructor
	ColorBlindFilter() : TTN_Scene() {}

	//default destrcutor
	~ColorBlindFilter() = default;

	//sets up the scene
	void InitScene() {
		glm::ivec2 windowSize = TTN_Backend::GetWindowSize();
		m_colorCorrection = TTN_ColorCorrect::Create();
		m_colorCorrection->Init(windowSize.x, windowSize.y);
		m_colorCorrection->SetIntensity(1.0f);
		m_colorCorrection->SetCube(TTN_AssetSystem::GetLUT("Red / Green LUT"));
		m_colorCorrection->SetShouldApply(false);
		m_PostProcessingEffects.push_back(m_colorCorrection);

		entt::entity cam = CreateEntity();
		SetCamEntity(cam);
		Attach<TTN_Transform>(cam);
		Attach<TTN_Camera>(cam);
		auto& camTrans = Get<TTN_Transform>(cam);
		camTrans.SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
		camTrans.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		camTrans.LookAlong(glm::vec3(0.0, 0.0, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		Get<TTN_Camera>(cam).CalcOrtho(-960.0f, 960.0f, -540.0f, 540.0f, 0.0f, 10.0f);
		Get<TTN_Camera>(cam).View();
	}

	void TurnOff() {
		m_colorCorrection->SetShouldApply(false);
	}
	void MakeRedGreen() {
		m_colorCorrection->SetCube(TTN_AssetSystem::GetLUT("Red/Green LUT"));
		m_colorCorrection->SetShouldApply(true);
	}
	void MakeBlueYellow() {
		m_colorCorrection->SetCube(TTN_AssetSystem::GetLUT("Blue/Yellow LUT"));
		m_colorCorrection->SetShouldApply(true);
	}

private:
	TTN_ColorCorrect::scolcorptr m_colorCorrection;
};