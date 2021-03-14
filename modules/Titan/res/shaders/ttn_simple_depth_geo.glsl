#version 410

//based on and modified from code found in https://youtu.be/lUo7s-i9Gy4 

layout(triangles, invocations = 4) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 u_LightSpaceMatrix[4];

void main()
{
	//go through each vertex, setting the approriate layer and grabbing the approraite light space matrix based on the invocation id so it renders to all the shadow maps and can discard from ones it's not in later down the pipeline

	//first vertex
	gl_Layer = gl_InvocationID;
	gl_Position = u_LightSpaceMatrix[gl_InvocationID] * gl_in[0].gl_Position;
	EmitVertex();

	//second vertex
	gl_Layer = gl_InvocationID;
	gl_Position = u_LightSpaceMatrix[gl_InvocationID] * gl_in[1].gl_Position;
	EmitVertex();

	//third vertex
	gl_Layer = gl_InvocationID;
	gl_Position = u_LightSpaceMatrix[gl_InvocationID] * gl_in[2].gl_Position;
	EmitVertex();

	//when it has processed all the vertices in the triangle 
	EndPrimitive();
}