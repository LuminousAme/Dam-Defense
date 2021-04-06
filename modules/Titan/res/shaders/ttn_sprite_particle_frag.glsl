#version 410

//mesh data from vert shader
layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inColor;

//material data
uniform sampler2D s_Diffuse;

//result 
out vec4 frag_color;

void main() {
	vec4 textCol = texture(s_Diffuse, inUV);

	vec3 result = inColor.rgb * textCol.rgb;

	//save the result and pass it on
	frag_color = vec4(result, textCol.a * inColor.a);
}