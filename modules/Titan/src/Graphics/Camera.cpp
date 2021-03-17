//Titan Engine, by Atlas X Games 
// Camera.cpp - source file for the class that represents a camera in a 3D scene

#include "Titan/ttn_pch.h"
//include the precompile header, this file uses GLM/gtx/transform.hpp, and iostream
#include "Titan/Graphics/Camera.h"

namespace Titan {
		

	TTN_Camera::TTN_Camera(){
		m_position = (glm::vec3(0.0f)); //origin
		m_forward = (glm::vec3(0.0f, 0.0f, 1.0f));
		m_up = (glm::vec3(0.0f, 1.0f, 0.0f)); // Use the Y-up coordinate system by default
		m_view = (glm::mat4(1.0f));
		m_projection = (glm::mat4(1.0f));
		m_viewProjection = (glm::mat4(1.0f));
		m_target = m_forward; //camera facing forward
		m_above = (glm::vec3(1.0f, 0.0f, 0.0f)); //x axis

	}

	void TTN_Camera::View(){
		m_view = glm::lookAt(m_position, m_target, m_up); //lookAt function uses position, target and up vector as paramaters
	}

	void TTN_Camera::LookAt(const glm::vec3& point){
		m_forward = -glm::normalize(m_position - point);
	}

	const glm::mat4& TTN_Camera::GetVP(){
		m_viewProjection = m_projection * m_view;
		return m_viewProjection;
	}

	void TTN_Camera::SetPosition(const glm::vec3& position){
		m_position = position;
	}

	void TTN_Camera::SetForward(const glm::vec3& forward){
		m_forward = forward;
	}

	void TTN_Camera::SetUp(const glm::vec3& up){
		m_up = up;
	}

	void TTN_Camera::SetTarget(const glm::vec3& target)
	{
		m_target = target;
	}

	void TTN_Camera::CalcOrtho(float left, float right, float bottom, float top, float nearClip, float farClip){
		m_projection = glm::ortho(left, right, bottom, top, nearClip, farClip);
	}

	void TTN_Camera::CalcPerspective(float fovDegrees, float aspectRatio, float nearClip, float farClip){
		m_projection = glm::perspective(glm::radians(fovDegrees), aspectRatio, nearClip, farClip);
		m_fov = fovDegrees;
		m_aspectRatio = aspectRatio;
		m_zNear = nearClip;
		m_zFar = farClip;
	}

	std::vector<glm::vec3> TTN_Camera::CalcPerspectiveCorners(glm::vec3 center, glm::vec3 forward, glm::vec3 right, glm::vec3 up, float nearT, float farT)
	{
		//get the tangent of half of the fov in degrees
		float tfov = glm::tan(glm::radians(m_fov / 2.0f));

		float nearClipping = glm::mix(m_zNear, m_zFar, nearT);
		float farClipping = glm::mix(m_zNear, m_zFar, farT);

		//get the near plane's center, width, and height
		float hn = 2.0f * tfov * nearClipping;
		float wn = hn * m_aspectRatio;
		glm::vec3 cn = center + glm::normalize(forward) * nearClipping;

		//get the far plane's center, width, and height
		float hf = 2.0f * tfov * farClipping;
		float wf = hf * m_aspectRatio;
		glm::vec3 cf = center + glm::normalize(forward) * farClipping;

		//calculate the corners
		glm::vec3 ntl = cn + glm::normalize(up) * hn / 2.0f - glm::normalize(right) * wn / 2.0f;
		glm::vec3 ntr = cn + glm::normalize(up) * hn / 2.0f + glm::normalize(right) * wn / 2.0f;
		glm::vec3 nbl = cn - glm::normalize(up) * hn / 2.0f - glm::normalize(right) * wn / 2.0f;
		glm::vec3 nbr = cn - glm::normalize(up) * hn / 2.0f + glm::normalize(right) * wn / 2.0f;

		glm::vec3 ftl = cf + glm::normalize(up) * hf / 2.0f - glm::normalize(right) * wf / 2.0f;
		glm::vec3 ftr = cf + glm::normalize(up) * hf / 2.0f + glm::normalize(right) * wf / 2.0f;
		glm::vec3 fbl = cf - glm::normalize(up) * hf / 2.0f - glm::normalize(right) * wf / 2.0f;
		glm::vec3 fbr = cf - glm::normalize(up) * hf / 2.0f + glm::normalize(right) * wf / 2.0f;

		//make a vector and push all the corners back into the vector
		std::vector<glm::vec3> corners = std::vector<glm::vec3>();

		corners.push_back(ntl);
		corners.push_back(ntr);
		corners.push_back(nbl);
		corners.push_back(nbr);

		corners.push_back(ftl);
		corners.push_back(ftr);
		corners.push_back(fbl);
		corners.push_back(fbr);

		//finally return that vector
		return corners;
	}
}