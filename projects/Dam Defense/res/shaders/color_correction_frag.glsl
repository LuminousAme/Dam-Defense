#version 420

layout(location = 0) in vec2 inUV;

out vec4 frag_color;

layout(binding = 0) uniform sampler2D u_FinishedFrame;
layout(binding = 30) uniform sampler3D u_TexColorGrade;


void main() {
	
	vec4 textureColor = texture(u_FinishedFrame, inUV);
	vec3 scale = vec3((64.0-1.0)/64.0 );
	vec3 offset = vec3(1.0/(2.0*64.0));
	
	frag_color.rgb = texture(u_TexColorGrade, scale * textureColor.rgb + offset).rgb;
	frag_color.a = textureColor.a;
}