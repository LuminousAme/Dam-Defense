#version 410

//mesh data from vert shader
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;

//material data
uniform sampler2D s_Diffuse;

//result 
out vec4 frag_color;

void main() {
	vec4 textCol = texture(s_Diffuse, inUV);

	if(textCol.a < 0.05)
		discard;

	vec3 result = inColor * textCol.rgb;

	//save the result and pass it on
	frag_color = vec4(result, textCol.a);
}