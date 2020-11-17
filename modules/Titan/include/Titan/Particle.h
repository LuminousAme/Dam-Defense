#pragma once

#include "Titan/Application.h"
#include "Titan/Physics.h"
#include "Titan/ObjLoader.h"
#include "Titan/Renderer.h"
#include "GLM/glm.hpp"

#include <vector>
#include <iostream>

namespace Titan {

	struct ParticleData
	{
		glm::vec3 m_position;
		glm::vec3 m_velocity, VelocityVariation;
		glm::vec4 ColorBegin, ColorEnd;
		float SizeBegin, SizeEnd, SizeVariation;
		float LifeTime = 1.0f;
	};

	class TTN_Particle
	{
	public:
		TTN_Particle();

		void OnUpdate(float deltaTime);
		void OnRender();

		void Emit(const ParticleData& particleProps);
	private:
		struct Particle
		{
			glm::vec3 Position;
			glm::vec3 Velocity;
			glm::vec4 ColorBegin, ColorEnd;
			float Rotation = 0.0f;
			float SizeBegin, SizeEnd;

			float LifeTime = 1.0f;
			float LifeRemaining = 0.0f;

			bool Active = false;
		};
		std::vector<Particle> m_ParticlePool;
		uint32_t m_PoolIndex = 999;

		GLuint m_QuadVA = 0;
		//std::unique_ptr<GLCore::Utils::Shader> m_ParticleShader;
		//GLint m_ParticleShaderViewProj, m_ParticleShaderTransform, m_ParticleShaderColor;
	};







	//struct ParticleData {
	//	glm::vec3 m_pos;
	//	glm::vec3 m_velo; // Current particle velocity
	//	glm::vec4 m_colorStart, m_colorEnd;   // Particle color

	//	float m_size, m_sizeEnd;
	//	float m_speed;

	//	float m_life;
	//	float m_angle;

	//	//default constructor for struct
	//	ParticleData() {
	//		m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
	//		m_velo = glm::vec3(0.0f, 1.0f, 0.0f);
	//		m_colorStart = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//		m_colorEnd = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	//		m_size = 1.0f;
	//		m_sizeEnd = 0.0f;
	//		m_speed = 2.0f;
	//		m_life = 5.0f;
	//		m_angle = glm::tan(glm::radians(25.0f));
	//	}

	//};


	//class TTN_Particle
	//{
	//public:
	//	TTN_Particle();
	//	virtual ~TTN_Particle() = default;

	//	TTN_Particle(const ParticleData& param);

	//	void Update(float deltatime);
	//	void Draw();
	//	void Render();

	//private:

	//	struct Particle {
	//		glm::vec3 m_pos;
	//		glm::vec3 m_velo; // Current particle velocity
	//		glm::vec4 m_colorStart, m_colorEnd;    // Particle color

	//		float m_size, m_sizeEnd;
	//		float m_speed;

	//		float m_life;
	//		float m_angle;
	//		float m_lifeRemain = 0.0f;
	//		bool* alive = false;

	//		Particle(const ParticleData& p);
	//		~Particle();

	//	};

	//	std::vector<Particle> m_ParticlePool;
	//	std::unique_ptr<Particle> m_data;


	//};

}