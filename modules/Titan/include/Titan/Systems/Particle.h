//Titan Engine, by Atlas X Games 
// Particle.h - header for the class that represents a particle system
#pragma once

//precompile header, this file uses GLM/glm.hpp, GLM/gtx/quaternion.hpp, vector, iostream, and algorithm
#include "Titan/ttn_pch.h"
//include other titan featues
#include "Titan/Graphics/ObjLoader.h"
#include "Titan/Graphics/Renderer.h"
#include "Titan/Utilities/Random.h"

namespace Titan {
	//enum for the particle emitter type
	enum class TTN_ParticleEmitterShape {
		CONE = 0,
		SPHERE = 1,
		CIRCLE = 2,
		CUBE = 3
	};

	struct TTN_ParticleTemplate {
		glm::vec4 _StartColor, _StartColor2;
		glm::vec4 _EndColor, _EndColor2;
		float _StartSize, _StartSize2;
		float _EndSize, _EndSize2;
		float _startSpeed, _startSpeed2;
		float _endSpeed, _endSpeed2;
		float _lifeTime, _lifeTime2;
		TTN_Mesh::smptr _mesh;
		TTN_Material::smatptr _mat;

		TTN_ParticleTemplate()
		{
			_StartColor = glm::vec4(1.0f);
			_StartColor2 = glm::vec4(1.0f);
			_EndColor = glm::vec4(1.0f);
			_EndColor2 = glm::vec4(1.0f);
			_StartSize = 1.0f;
			_StartSize2 = 1.0f;
			_EndSize = 1.0f;
			_EndSize2 = 1.0f;
			_startSpeed = 1.0f;
			_startSpeed2 = 1.0f;
			_endSpeed = 1.0f;
			_endSpeed2 = 1.0f;
			_lifeTime = 1.0f;
			_lifeTime2 = 1.0f;
			_mesh = TTN_Mesh::Create();
			_mat = TTN_Material::Create();
		}

		void SetOneStartColor(glm::vec4 startColor) {
			_StartColor = startColor;
			_StartColor2 = startColor;
		}
		void SetTwoStartColors(glm::vec4 startColor, glm::vec4 startColor2) {
			_StartColor = startColor;
			_StartColor2 = startColor2;
		}

		void SetOneEndColor(glm::vec4 endColor) {
			_EndColor = endColor;
			_EndColor2 = endColor;
		}
		void SetTwoEndColors(glm::vec4 endColor, glm::vec4 endColor2) {
			_EndColor = endColor;
			_EndColor2 = endColor2;
		}

		void SetOneStartSize(float startSize) {
			_StartSize = startSize;
			_StartSize2 = startSize;
		}
		void SetTwoStartSizes(float startSize, float startSize2) {
			_StartSize = startSize;
			_StartSize2 = startSize2;
		}

		void SetOneEndSize(float endSize) {
			_EndSize = endSize;
			_EndSize2 = endSize;
		}
		void SetTwoEndSizes(float endSize, float endSize2) {
			_EndSize = endSize;
			_EndSize2 = endSize2;
		}

		void SetOneStartSpeed(float StartSpeed) {
			_startSpeed = StartSpeed;
			_startSpeed2 = StartSpeed;
		}
		void SetTwoStartSpeeds(float StartSpeed, float StartSpeed2) {
			_startSpeed = StartSpeed;
			_startSpeed2 = StartSpeed2;
		}

		void SetOneEndSpeed(float endSpeed) {
			_endSpeed = endSpeed;
			_endSpeed2 = endSpeed;
		}
		void SetTwoEndSpeeds(float endSpeed, float endSpeed2) {
			_endSpeed = endSpeed;
			_endSpeed2 = endSpeed2;
		}

		void SetOneLifetime(float LifeTime) {
			_lifeTime = LifeTime;
			_lifeTime2 = LifeTime;
		}
		void SetTwoLifetimes(float LifeTime, float LifeTime2) {
			_lifeTime = LifeTime;
			_lifeTime2 = LifeTime2;
		}

		void SetMesh(TTN_Mesh::smptr mesh) {
			_mesh = mesh;
		}
		void SetMat(TTN_Material::smatptr mat) {
			_mat = mat;
		}
	};

	//class for a particle system
	class TTN_ParticleSystem {
	public:
		//defines a special easier to use name for shared(smart) pointers to the class 
		typedef std::shared_ptr<TTN_ParticleSystem> spsptr;

		//creates and returns a shared(smart) pointer to the class 
		static inline spsptr Create() {
			return std::make_shared<TTN_ParticleSystem>();
		}

	public:
		//ensuring moving and copying is not allowed so we can control destructor calls through pointers
		TTN_ParticleSystem(const TTN_ParticleSystem& other) = delete;
		TTN_ParticleSystem(TTN_ParticleSystem& other) = delete;
		TTN_ParticleSystem& operator=(const TTN_ParticleSystem& other) = delete;
		TTN_ParticleSystem& operator=(TTN_ParticleSystem&& other) = delete;

	public:
		//default constructor
		TTN_ParticleSystem();

		//Constructor that takes data
		TTN_ParticleSystem(size_t maxParticles, float emissionRate, TTN_ParticleTemplate particleTemplate,
			float duration = 0.0f, bool loop = true);

		//default destructor
		~TTN_ParticleSystem();

		//setsup the shader, called by titan's application init
		static void InitParticleShader();

		//makes it a sprite based particle system
		void MakeParticlesAsSprites(TTN_Texture2D::st2dptr spriteTexture) 
		{ 
			m_spriteParticleTexture = spriteTexture; 
		m_isSprites = true; 
		s_spriteVAO = TTN_VertexArrayObject::Create();

		s_spriteVAO->AddVertexBuffer(s_spriteVertexPosVBO, { BufferAttribute(0, 3, GL_FLOAT, false, sizeof(float) * 3, 0, AttribUsage::Position) });
		s_spriteVAO->AddVertexBuffer(ColorInstanceBuffer, { BufferAttribute(1, 4, GL_FLOAT, false, sizeof(float) * 4, 0, AttribUsage::Color, 1) });
		s_spriteVAO->AddVertexBuffer(PositionInstanceBuffer, { BufferAttribute(2, 3, GL_FLOAT, false, sizeof(float) * 3, 0, AttribUsage::User0, 1) });
		s_spriteVAO->AddVertexBuffer(ScaleInstanceBuffer, { BufferAttribute(3, 1, GL_FLOAT, false, sizeof(float), 0, AttribUsage::User1, 1) });
		}
		void MakeParticlesAsFullMesh() { m_isSprites = false; }

		//sets up emitter data
		void MakeConeEmitter(float angle, glm::vec3 emitterRotation = glm::vec3(0.0f));
		void MakeCircleEmitter(glm::vec3 emitterRotation = glm::vec3(0.0f));
		void MakeSphereEmitter();
		void MakeCubeEmitter(glm::vec3 scale = glm::vec3(1.0f), glm::vec3 emitterRotation = glm::vec3(0.0f));

		//setters
		void SetEmitterAngle(float angle);
		void SetEmitterScale(glm::vec3 scale);
		void SetDuration(float duration);
		void SetShouldLoop(bool shouldLoop);
		void SetParticleTemplate(TTN_ParticleTemplate particleTemplate);
		void SetEmissionRate(float emissionRate);
		void SetEmitterRotation(glm::vec3 rotation);
		void SetPaused(bool paused);

		//getters
		float GetEmitterAngle() { return m_EmitterAngle; }
		glm::vec3 GetEmitterScale() { return m_EmitterScale; }
		float GetDuration() { return m_duration; }
		bool GetShouldLoop() { return m_loop; }
		TTN_ParticleTemplate GetParticleTemplate() { return m_particle; }
		float GetEmissionRate() { return m_emissionRate; }
		glm::vec3 GetEmitterRotation() { return glm::degrees(m_rotation); }
		bool GetPaused() { return m_paused; }

		//function pointer setters
		void VelocityReadGraphCallback(float (*function)(float));
		void ColorReadGraphCallback(float (*function)(float));
		void RotationReadGraphCallback(float (*function)(float));
		void ScaleReadGraphCallback(float (*function)(float));

		//updates the particle system as a whole, as well as the all the indivual particles 
		void Update(float deltaTime);

		//renders all the particles
		void Render(glm::vec3 ParentGlobalPos, glm::mat4 view, glm::mat4 projection);

		//emits a single particle
		void Emit();

		//emits that number of particles at that time
		void Burst(size_t numOfParticles);

	private:
		//particle artibutes
		glm::vec3* Positions;

		glm::vec4* StartColors;
		glm::vec4* EndColors;

		glm::vec3* StartVelocities;
		glm::vec3* EndVelocities;

		float* StartScales;
		float* EndScales;

		float* timeAlive;
		float* lifeTimes;
		bool* Active;

		//render data
		glm::vec3* particle_pos;
		glm::vec4* particle_col;
		float* particle_scale;

		//setable system data
		glm::vec3 m_rotation;
		TTN_ParticleEmitterShape m_emitterShape;
		float m_EmitterAngle;
		glm::vec3 m_EmitterScale;
		float m_emissionRate;
		TTN_ParticleTemplate m_particle;
		float m_duration;
		bool m_loop;
		float m_emissionTimer;
		bool m_paused;

		//other data
		size_t m_activeParticleIndex;
		float m_durationRemaining;		
		size_t m_maxParticlesCount;
		inline static TTN_Shader::sshptr s_particleShaderProgram;
		inline static TTN_Texture2D::st2dptr s_defaultWhiteTexture;
		TTN_VertexArrayObject::svaptr m_vao;
		TTN_VertexBuffer::svbptr VertexPosVBO;
		TTN_VertexBuffer::svbptr VertexNormVBO;
		TTN_VertexBuffer::svbptr VertexUVVBO;
		TTN_VertexBuffer::svbptr ColorInstanceBuffer;
		TTN_VertexBuffer::svbptr PositionInstanceBuffer;
		TTN_VertexBuffer::svbptr ScaleInstanceBuffer;
		//stuff for sprite particles
		TTN_VertexArrayObject::svaptr s_spriteVAO;
		inline static TTN_VertexBuffer::svbptr s_spriteVertexPosVBO;
		inline static glm::vec3 s_particleSpriteVertexPos = glm::vec3(0.0f);
		inline static TTN_Shader::sshptr s_spriteParticleShader;
		TTN_Texture2D::st2dptr m_spriteParticleTexture;
		bool m_isSprites = false;

		//function pointers for lerp
		float (*readGraphVelo)(float);
		float (*readGraphColor)(float);
		float (*readGraphRotation)(float);
		float (*readGraphScale)(float);

		void SetUpRenderingStuff();
		void SetUpData();
	};

	//class for a particle system compomenet
	class TTN_ParticeSystemComponent {
	public:
		TTN_ParticeSystemComponent() = default;
		~TTN_ParticeSystemComponent() = default;

		TTN_ParticeSystemComponent(TTN_ParticleSystem::spsptr ParticleSystem) {
			ps = ParticleSystem;
		}

		void SetParticleSystemPointer(TTN_ParticleSystem::spsptr ParticleSystem) {
			ps = ParticleSystem;
		}

		TTN_ParticleSystem::spsptr GetParticleSystemPointer() { return ps; }

	private:
		TTN_ParticleSystem::spsptr ps;
	};

	//default readgraph
	inline float defaultReadGraph(float t) {
		return t;
	}
}