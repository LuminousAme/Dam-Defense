#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GLM/glm.hpp"
namespace Titan {

	class TTN_Camera
	{
	public:

		TTN_Camera();
		virtual ~TTN_Camera() = default;

		void View(const glm::vec3& point); // change view to this point
		void LookAt(const glm::vec3& point); // changes the camera direction to this point

		//GETTERS
		//gets camera's position in the world
		const glm::vec3& GetPosition() { return m_position; }
		/// Gets the direction that this camera is facing in world space
		const glm::vec3& GetForward() { return m_forward; }
		/// Gets the up direction for this camera (basically a vector pointing out the top of the camera's head)
		const glm::vec3& GetUp() { return m_up; }

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


		// orthographic projection 
		void CalcOrtho(float left, float right, float bottom, float top, float near, float far);

		// 3D perspective projection 
		void CalcPerspective(float fovDegrees, float aspectRatio, float near, float far);

	protected:
			
		glm::vec3 m_position;  //postion of camera in world space
		glm::vec3 m_forward; // the direction the camera will be facing (forward vector)
		glm::vec3 m_up; // camera's up vector (vector sticking upwards from camera's head

		glm::mat4 m_view;
		glm::mat4 m_projection;

		mutable glm::mat4 m_viewProjection;//mutable, so it can be recalculated using const methods
	};
}

