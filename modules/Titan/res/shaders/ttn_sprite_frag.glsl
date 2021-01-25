#version 410

//mesh data from the vert shader
layout(location = 0) in vec2 inUv;

//material data
uniform sampler2D s_Diffuse;

//result
out vec4 frag_color;

void main() {
	//set the fragment color from the texture 
	frag_color = texture(s_Diffuse, inUv);
}