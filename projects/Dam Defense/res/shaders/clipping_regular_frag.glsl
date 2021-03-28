#version 410

//in coming data from the vertex shader 
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;

//pass out fragment color 
out vec4 frag_Color;

//material data
uniform sampler2D s_Diffuse;
uniform int u_UseDiffuse;

void main() {
	//get the albedo from the diffuse / abledo texture map and output it
	frag_Color = mix(vec4(inColor, 1.0), texture(s_Diffuse, inUV) * vec4(inColor, 1.0), u_UseDiffuse);
}