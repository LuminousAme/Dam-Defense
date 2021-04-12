#version 410

//mesh data from the vert shader
layout(location = 0) in vec2 inUv;

//material data
uniform vec4 u_Color;
uniform sampler2D s_Diffuse;

uniform float u_VerticalMask;
uniform int u_startMaskAtBottom;

uniform float u_HorizontalMask;
uniform int u_startMaskOnRight;

//result
out vec4 frag_color;

void main() {
	vec4 New_Color = texture(s_Diffuse, inUv) * u_Color;

	if((inUv.x > u_HorizontalMask && u_startMaskOnRight == 0) || (inUv.y > u_VerticalMask && u_startMaskAtBottom == 0))
		discard;

	vec2 flippedUvs = vec2(0.0);
	flippedUvs.x = mix(1.0, 0.0, inUv.x);
	flippedUvs.y = mix(1.0, 0.0, inUv.y);

	if((flippedUvs.x > u_HorizontalMask && u_startMaskOnRight == 1) || (flippedUvs.y > u_VerticalMask && u_startMaskAtBottom == 1))
		discard;

	//set the fragment color from the texture 
	frag_color = New_Color;
}