#include "Titan/Particle.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/compatibility.hpp>
#include <GLM/gtx/transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include "glm/ext.hpp"

namespace Titan {



	TTN_Particle::TTN_Particle()
	{
		m_ParticlePool.resize(1000);
	}

	float TTN_Particle::lerp(float a, float b, float f)
	{
			return a + f * (b - a);
	}

	void TTN_Particle::Update(float deltatime)
	{
		for (auto& particle : m_ParticlePool)
		{
			if (!particle.Active)
				continue;

			if (particle.LifeRemaining <= 0.0f)
			{
				particle.Active = false;

				continue;
			}

			particle.LifeRemaining -= deltatime;
			particle.Position += particle.Velocity * deltatime;
			particle.Rotation += 0.01f * deltatime;

			if (particle.LifeRemaining <= 0.0f)
			{
				particle.LifeRemaining = 0.0f;
				continue;
			}

			// Fade away particles
			float life = particle.LifeRemaining / particle.LifeTime;
			glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);

			//color.a = color.a * life;
			float size = lerp(particle.SizeEnd, particle.SizeBegin, life);

			// Render
		/*	glm::mat4 transform = glm::translate(glm::mat4(1.0f), { particle.Position.x, particle.Position.y, 0.0f })
				* glm::rotate(glm::mat4(1.0f), particle.Rotation, { 0.0f, 0.0f, 1.0f })
				* glm::scale(glm::mat4(1.0f), { size, size, 1.0f });*/

			//std::cout << glm::to_string(transform) << std::endl;

			glBindVertexArray(m_QuadVA);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
			
		}
		
	}

	void TTN_Particle::Render()
	{
		if (!m_QuadVA) {
			std::cout << "Working" << std::endl;

			float vertices[] = {
				 -0.5f, -0.5f, 0.0f,
				  0.5f, -0.5f, 0.0f,
				  0.5f,  0.5f, 0.0f,
				 -0.5f,  0.5f, 0.0f
			};

			glCreateVertexArrays(1, &m_QuadVA);
			glBindVertexArray(m_QuadVA);

			GLuint quadVB, quadIB;
			glCreateBuffers(1, &quadVB);
			glBindBuffer(GL_ARRAY_BUFFER, quadVB);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glEnableVertexArrayAttrib(quadVB, 0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

			uint32_t indices[] = {
				0, 1, 2, 2, 3, 0
			};

			glCreateBuffers(1, &quadIB);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		}

		//for (auto& particle : m_ParticlePool)
		//{
		//	if (!particle.Active)
		//		continue;

		//	// Fade away particles
		//	float life = particle.LifeRemaining / particle.LifeTime;
		//	glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);
		//	//color.a = color.a * life;
		//	float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);

		//	// Render
		//	glm::mat4 transform = glm::translate(glm::mat4(1.0f), { particle.Position.x, particle.Position.y, 0.0f })
		//		* glm::rotate(glm::mat4(1.0f), particle.Rotation, { 0.0f, 0.0f, 1.0f })
		//		* glm::scale(glm::mat4(1.0f), { size, size, 1.0f });
		//
		//	glBindVertexArray(m_QuadVA);
		//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		//}
	}

	void TTN_Particle::Emit(const ParticleData& particleProps) {
		Particle& particle = m_ParticlePool[m_PoolIndex];
		particle.Active = true;
		particle.Position = particleProps.m_position;
		particle.Rotation = glm::tan(glm::radians(25.0f));
		
		// Velocity
		particle.Velocity = particleProps.m_velocity;
		//particle.Velocity.x += particleProps.VelocityVariation.x ;
		//particle.Velocity.y += particleProps.VelocityVariation.y;

		// Color
		particle.ColorBegin = particleProps.ColorBegin;
		particle.ColorEnd = particleProps.ColorEnd;

		particle.LifeTime = particleProps.LifeTime;
		particle.LifeRemaining = particleProps.LifeTime;
		particle.SizeBegin = particleProps.SizeBegin ;
		particle.SizeEnd = particleProps.SizeEnd;

		m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
	}


	//TTN_Particle::TTN_Particle() {
	//	m_data = nullptr;
	//}

	//TTN_Particle::TTN_Particle(const ParticleData& param)
	//{
	//	m_data = std::make_unique<Particle>(param);
	//}

	//void TTN_Particle::Update(float deltatime)
	//{
	//	for (auto& particle : m_ParticlePool)
	//	{


	//		if (particle.m_life <= 0.0f)
	//		{
	//			//particle.Active = false;
	//			continue;
	//		}

	//		particle.m_life -= deltatime;
	//		particle.m_pos += particle.m_velo * (float)deltatime;
	//		particle.m_angle += 0.01f * deltatime;
	//		//particle.m_color += 
	//	}

	//}

	//void TTN_Particle::Render()
	//{
	//	// The VBO containing the 4 vertices of the particles.
	//// Thanks to instancing, they will be shared by all particles.
	//	static GLfloat g_vertex_buffer_data[] = {
	//	 -0.5f, -0.5f, 0.0f,
	//	 0.5f, -0.5f, 0.0f,
	//	 -0.5f, 0.5f, 0.0f,
	//	 0.5f, 0.5f, 0.0f,
	//	};

	//	GLuint billboard_vertex_buffer;
	//	glGenBuffers(1, &billboard_vertex_buffer);
	//	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	//	// The VBO containing the positions and sizes of the particles
	//	GLuint particles_position_buffer;
	//	glGenBuffers(1, &particles_position_buffer);
	//	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	//	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	//	glBufferData(GL_ARRAY_BUFFER,  sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	//	// The VBO containing the colors of the particles
	//	GLuint particles_color_buffer;
	//	glGenBuffers(1, &particles_color_buffer);
	//	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	//	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	//	glBufferData(GL_ARRAY_BUFFER,  sizeof(GLubyte), NULL, GL_STREAM_DRAW);

	//	for (auto& particle : m_ParticlePool)
	//	{
	//		// Fade away particles
	//		float life = particle.m_lifeRemain / particle.m_life;
	//		glm::vec4 color = glm::lerp(particle.m_colorEnd, particle.m_colorStart, life);
	//		//color.a = color.a * life;

	//		float size = glm::lerp(particle.m_sizeEnd, particle.m_size, life);

	//		// Render
	//		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { particle.m_pos.x, particle.m_pos.y, 0.0f })
	//			* glm::rotate(glm::mat4(1.0f), particle.m_angle, { 0.0f, 0.0f, 1.0f })
	//			* glm::scale(glm::mat4(1.0f), { size, size, 1.0f });

	//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	//	}



	//}
	//TTN_Particle::Particle::Particle(const ParticleData& p)
	//{
	//	m_pos = p.m_pos;
	//	m_velo = p.m_velo;

	//	m_colorStart = p.m_colorStart;
	//	m_colorEnd = p.m_colorEnd;

	//	m_speed = p.m_speed;
	//	m_life = p.m_life;
	//	m_angle = p.m_angle;
	//	alive = false;
	//	m_lifeRemain = 0.0f;

	//}
	//TTN_Particle::Particle::~Particle()
	//{
	//	free(alive);
	//}
}
