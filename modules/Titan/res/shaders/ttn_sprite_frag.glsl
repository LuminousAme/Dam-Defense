#version 410

//mesh data from the vert shader
layout(location = 0) in vec2 inUv;

//material data
uniform vec4 u_Color;
uniform sampler2D s_Diffuse;

//result
out vec4 frag_color;

void main() {
	vec4 New_Color = texture(s_Diffuse, inUv) * u_Color;

	//set the fragment color from the texture 
	frag_color = New_Color;
}