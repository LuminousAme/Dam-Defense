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

		void SetViewMat(glm::mat4 view);
		void SetLightSpaceMatrices(glm::mat4 mats[]);
		void SetSplitRanges(float splits[]);
		void SetFarClip(float farClip);
		void SetCamPos(glm::vec3 camPos);
		void SetShadowBuffer(TTN_Framebuffer::sfboptr shadowBuffer);
		void SetRimColor(glm::vec3 rimColor) { m_rimColor = rimColor; }
		void SetRimSize(float rimSize) { m_rimSize = rimSize; }

		TTN_DirectionalLight& GetSunRef();

		//Sets the sun in the scene
		void SetSun(TTN_DirectionalLight newSun);
		void SetSun(glm::vec4 lightDir, glm::vec4 lightCol);

		//sets variables for toon shading
		void SetDiffuseRamp(TTN_Texture2D::st2dptr diffuseRamp) { m_diffuseRamp = diffuseRamp; }
		void SetUseDiffuseRamp(bool useDiffuseRamp) { m_useDiffuseRamp = useDiffuseRamp; }
		void SetSpecularRamp(TTN_Texture2D::st2dptr specularRamp) { m_specularRamp = specularRamp; }
		void SetUseSpecularRamp(bool useSpecularRamp) { m_useSpecularRamp = useSpecularRamp; }


		void EnableSun(bool enabled);

		glm::vec3 GetRimColor() { return m_rimColor; }
		float GetRimSize() { return m_rimSize; }

	private:
		glm::mat4 m_viewMat;
		glm::mat4 m_lightSpaceViewProj[4];
		float m_splitRanges[4];
		float m_farClip;
		glm::vec3 m_camPos;
		glm::vec3 m_rimColor = glm::vec3(1.0f, 1.0f, 1.0f);
		float m_rimSize = 0.4f;

		TTN_Framebuffer::sfboptr m_shadowBuffer;

		TTN_UniformBuffer m_sunBuffer;

		bool m_sunEnabled = true;

		TTN_DirectionalLight m_sun;

		TTN_Texture2D::st2dptr m_diffuseRamp;
		TTN_Texture2D::st2dptr m_specularRamp;
		bool m_useDiffuseRamp;
		bool m_useSpecularRamp;
	};
}