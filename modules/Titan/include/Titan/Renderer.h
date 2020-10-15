//Titan Engine, by Atlas X Games 
// Renderer.h - header for the class that allows meshes, 2D sprites, etc. be rendered 
#pragma once

//include the opengl wrapper classes
#include "Shader.h"
#include "VertexArrayObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Titan {

	//class that acts as a component to allow objects to be rendered in the game 
	class TTN_Renderer {
	public:
		//enum class to be explcit wheter something is a 3D mesh, a 2D sprite, etc.
		enum class RenderObjectType {
			SPRITE = 1, 
			MESH
		};



	protected:
		//a pointer to the shader that should be used to render this object
		TTN_Shader* shader;
	};
}