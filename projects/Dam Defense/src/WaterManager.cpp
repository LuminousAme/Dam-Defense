//Dam Defense by Atlas X Games
//WaterManager.cpp, the source file for the class that manages the water effect
#include "WaterManager.h"

void WaterManager::Init()
{
	m_waveFrequency.clear();
	m_waveDirection.clear();
	m_waveAcutalAmplitude.clear();
	m_waveTargetAmplitude.clear();
	m_waveTimeAlive.clear();

	for (int i = 0; i < m_numberOfWaves; i++) {
		//generate a wavelenght
		float lenght = TTN_Random::RandomFloat(m_medianWaveLenght * 0.75f, m_medianWaveLenght * 1.25f);
		//calculate the frequency based on the wavelenght
		float freq = glm::sqrt(m_gravationalConstant * ((2.0f * glm::pi<float>()) / lenght));

		//get the ratio between the median amplitude and wave lenght
		float ratio = m_medianAmplitude / m_medianWaveLenght;
		//use that to calculate the target amplitude
		float targetAmp = ratio * lenght;

		//use the current iteration, number of total waves, and the total life time to calculate how far into
		//it's life each wave currently is, this allows us to displace waves fading in and out from each other
		float currentTimeAlive = i * (m_lifeTime / float(m_numberOfWaves));

		//take the base direction of the wave
		glm::vec3 dir = m_basedirection;

		//and generate a random rotation within a given angle to it 
		glm::vec3 Rot = glm::vec3(TTN_Random::RandomFloat(-m_angle, m_angle), 0.0f, TTN_Random::RandomFloat(-m_angle, m_angle));

		//and rotate it by that ammount
		glm::quat RotQuat = glm::quat(glm::radians(Rot));
		glm::mat4 RotMat = glm::toMat4(RotQuat);
		dir = glm::vec3(RotMat * glm::vec4(dir, 1.0f));

		//get the acutal amplitude
		float t = glm::clamp(currentTimeAlive / m_lifeTime, 0.0f, 1.0f);
		float acutalAmp = TTN_Interpolation::Lerp(0.0f, targetAmp, waterLifeInterpolationParameter(t));

		//push the values into the vectors
		m_waveFrequency.push_back(freq);
		m_waveDirection.push_back(glm::normalize(dir));
		m_waveAcutalAmplitude.push_back(acutalAmp);
		m_waveTargetAmplitude.push_back(targetAmp);
		m_waveTimeAlive.push_back(currentTimeAlive);
	}

	//set up voronoi stuff

	//make the framebuffer
	m_voronoiBuffer = TTN_Framebuffer::Create();
	m_voronoiBuffer->AddColorTarget(GL_RGBA8);
	m_voronoiBuffer->AddDepthTarget();
	m_voronoiBuffer->Init(1024, 1024);

	//make the shader
	m_voronoiShader = TTN_Shader::Create();
	m_voronoiShader->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_vert.glsl", GL_VERTEX_SHADER);
	m_voronoiShader->LoadShaderStageFromFile("shaders/voronoi_frag.glsl", GL_FRAGMENT_SHADER);
	m_voronoiShader->Link();
}

void WaterManager::MakeAWave(int index)
{
	//generate a wavelenght
	float lenght = TTN_Random::RandomFloat(m_medianWaveLenght * 0.5f, m_medianWaveLenght * 2.0f);
	//calculate the frequency based on the wavelenght
	float freq = glm::sqrt(m_gravationalConstant * ((2.0f * glm::pi<float>()) / lenght));

	//get the ratio between the median amplitude and wave lenght
	float ratio = m_medianAmplitude / m_medianWaveLenght;
	//use that to calculate the target amplitude
	float targetAmp = ratio * lenght;

	//use the current iteration, number of total waves, and the total life time to calculate how far into
	//it's life each wave currently is, this allows us to displace waves fading in and out from each other
	float currentTimeAlive = 0.0f;

	//take the base direction of the wave
	glm::vec3 dir = m_basedirection;

	//and generate a random rotation within a given angle to it 
	glm::vec3 Rot = glm::vec3(TTN_Random::RandomFloat(-m_angle, m_angle), 0.0f, TTN_Random::RandomFloat(-m_angle, m_angle));

	//and rotate it by that ammount
	glm::quat RotQuat = glm::quat(glm::radians(Rot));
	glm::mat4 RotMat = glm::toMat4(RotQuat);
	dir = glm::vec3(RotMat * glm::vec4(dir, 1.0f));

	//get the acutal amplitude
	float acutalAmp = 0.0f;

	//save the values into the vectors
	m_waveFrequency[index] = freq;
	m_waveDirection[index] = glm::normalize(dir);
	m_waveAcutalAmplitude[index] = acutalAmp;
	m_waveTargetAmplitude[index] = targetAmp;
	m_waveTimeAlive[index] = currentTimeAlive;
}

void WaterManager::Update(float deltaTime)
{
	//update the current time variable 
	m_time += deltaTime;

	//loop through each wave and update them 
	for (int i = 0; i < m_waveFrequency.size(); i++) {
		//update how long the wave has been alive
		m_waveTimeAlive[i] += deltaTime;

		//update it's acutal amplitude
		float t = glm::clamp(m_waveTimeAlive[i] / m_lifeTime, 0.0f, 1.0f);
		m_waveAcutalAmplitude[i] = TTN_Interpolation::Lerp(0.0f, m_waveTargetAmplitude[i], waterLifeInterpolationParameter(t));

		//if the wave's life time has ended, make a new wave to replace it
		if (m_waveTimeAlive[i] >= m_lifeTime)
			MakeAWave(i);
	}

	//if any waves have been added, reinit the system
	if (m_numberOfWaves != m_waveFrequency.size())
		Init();

	//render the voronoi effect
	m_voronoiBuffer->Clear();

	//bind the shader
	m_voronoiShader->Bind();
	
	//set the uniforms
	m_voronoiShader->SetUniform("u_gridFidelty", m_voronoiGridFidelty);
	m_voronoiShader->SetUniform("time", m_time);
	m_voronoiShader->SetUniform("u_speed", m_voronoiSpeed);

	//draw to the framebuffer
	m_voronoiBuffer->RenderToFSQ();

	//unbind the shader
	m_voronoiShader->UnBind();

	//and finally make sure the viewport is set correclty
	glm::ivec2 size = TTN_Backend::GetWindowSize();
	glViewport(0, 0, size.x, size.y);
}