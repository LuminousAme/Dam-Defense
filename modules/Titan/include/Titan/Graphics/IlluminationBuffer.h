#pragma once

//include the precompile header
#include "Titan/ttn_pch.h"
//include other titan graphics features
#include "Titan/Graphics/Texture2D.h"
#include "Titan/Graphics/Shader.h"
#include "Titan/Graphics/Post/PostEffect.h"
#include "Titan/Graphics/UniformBuffer.h"
#include "Titan/Graphics/GBuffer.h"
#include "Titan/Graphics/Light.h"

namespace Titan {
	enum TTN_Lights
	{
		DIRECTIONAL,
		AMBIENT
	};

	//This is a post effect to make our job easier
	class TTN_IlluminationBuffer : public TTN_PostEffect
	{
	public:
		//defines a special easier to use name for shared(smart) pointers to the class
		typedef std::shared_ptr<TTN_IlluminationBuffer> sillbufptr;

		//creates and returns a shared(smart) pointer to the class
		static inline sillbufptr Create() {
			return std::make_shared<TTN_IlluminationBuffer>();
		}

	public:
		//Initializes framebuffer
		//Overrides post effect Init
		void Init(unsigned width, unsigned height) override;

		//Makes it so apply effect with a PostEffect does nothing for this object
		void ApplyEffect(TTN_PostEffect::spostptr buffer) override { };
		//Can only apply effect using TTN_GBuffer object
		void ApplyEffect(TTN_GBuffer::sgbufptr gBuffer);

		void DrawIllumBuffer();

		void SetLightSpaceViewProj(glm::mat4 lightSpaceViewProj);
		void SetCamPos(glm::vec3 camPos);

		TTN_DirectionalLight& GetSunRef();

		//Sets the sun in the scene
		void SetSun(TTN_DirectionalLight newSun);
		void SetSun(glm::vec4 lightDir, glm::vec4 lightCol);

		void EnableSun(bool enabled);

	private:
		glm::mat4 m_lightSpaceViewProj;
		glm::vec3 m_camPos;

		TTN_UniformBuffer m_sunBuffer;

		bool m_sunEnabled = true;

		TTN_DirectionalLight m_sun;
	};
}