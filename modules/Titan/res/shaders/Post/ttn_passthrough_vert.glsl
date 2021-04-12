#version 410
//passthorguh vert

//data from c++ program
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inUV;

//data to pass on to the fragment shader
layout(location = 0) out vec2 outUV;

void main()
{ 
	//just pass on the uvs
	outUV = inUV;

	//and keep the fullscreen quad as is
	gl_Position = vec4(inPosition, 1.0);
}