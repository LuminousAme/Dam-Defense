//Titan Engine, by Atlas X Games 
// Camera.h - header for the class that represents a camera in a 3D scene
#pragma once

//precompile header, this file uses GLM/glm.hpp
#include "Titan/ttn_pch.h"

#define GLM_ENABLE_EXPERIMENTAL

namespace Titan {

	class TTN_Camera
	{
	public:

		TTN_Camera();
		virtual ~TTN_Camera() = default;

		void View(); // update the view
		void LookAt(const glm::vec3& point); // changes the camera direction to this point

		//GETTERS
		//gets camera's position in the world
		const glm::vec3& GetPosition() { return m_position; }
		/// Gets the direction that this camera is facing in world space
		const glm::vec3& GetForward() { return m_forward; }
		/// Gets the up direction for this camera (basically a vector pointing out the top of the camera's head)
		const glm::vec3& GetUp() { return m_up; }
		//Gets the far plane
		const float& GetFarPlane() { return m_zFar; }
		/// Gets the direction that the camera is looking at 
		const glm::vec3& GetTarget() { return m_target; }

		//gets the view and projection coordinates
		const glm::mat4& GetView() { return m_view; }
		const glm::mat4& GetProj() { return m_projection; }
		//Returns the viewprojection matrix for rendering.
		const glm::mat4& GetVP();

		//SETTERS
		//set camer'a world position coordinates
		void SetPosition(const glm::vec3& position);
		/// Sets the camera's forward vector, ie what way it's facing
		void SetForward(const glm::vec3& forward);
		/// Sets the camera's up vector, this can be thought of as a vector sticking out of the top of the camera's head
		void SetUp(const glm::vec3& up);
		///Sets what the camera is looking at 
		void SetTarget(const glm::vec3& target);

		// orthographic projection 
		void CalcOrtho(float left, float right, float bottom, float top, float nearClip, float farClip);

		// 3D perspective projection 
		void CalcPerspective(float fovDegrees, float aspectRatio, float nearClip, float farClip);

		// calculates and returns a vector with the corners
		std::vector<glm::vec3> CalcPerspectiveCorners(glm::vec3 center, glm::vec3 forward, glm::vec3 right, glm::vec3 up, float nearT, float farT);

	protected:
		glm::vec3 m_position;  //postion of camera in world space
		glm::vec3 m_forward; // the direction the camera will be facing (forward vector)
		glm::vec3 m_up; // camera's up vector (vector sticking upwards from camera's head
		
		glm::vec3 m_target; //what the camera should be looking at 

		glm::vec3 m_above; //x axis for camera to look up and down

		glm::mat4 m_view;
		glm::mat4 m_projection;

		float m_fov = 0.0f;
		float m_zNear = 0.0f;
		float m_zFar = 0.0f;
		float m_aspectRatio = 0.0f;

		mutable glm::mat4 m_viewProjection;//mutable, so it can be recalculated using const methods
	};
}