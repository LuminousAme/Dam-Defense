//Titan Engine, by Atlas X Games 
// Camera.cpp - source file for the class that represents a camera in a 3D scene

#include "Titan/Camera.h"
#include "GLM/gtx/transform.hpp"
#include <iostream>

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

	void TTN_Camera::keyInput(GLFWwindow* window) {
		const float m_speed = 0.1f; 
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			//m_target += m_speed * m_forward;
			m_target += glm::normalize(glm::cross(m_forward, m_above)) * m_speed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			//m_target -= m_speed * m_forward;
			m_target -= glm::normalize(glm::cross(m_forward, m_above)) * m_speed;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			m_target -= glm::normalize(glm::cross(m_forward, m_up)) * m_speed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			m_target += glm::normalize(glm::cross(m_forward, m_up)) * m_speed;
		}

		SetTarget(m_target);
		View();
	

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

	void TTN_Camera::CalcOrtho(float left, float right, float bottom, float top, float near, float far){
		m_projection = glm::ortho(left, right, bottom, top, near, far);
	}

	void TTN_Camera::CalcPerspective(float fovDegrees, float aspectRatio, float near, float far){
		m_projection = glm::perspective(glm::radians(fovDegrees), aspectRatio, near, far);
	}

}