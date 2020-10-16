#version 410

layout(location = 4) in vec3 inColour;

out vec4 frag_color;

void main() {
	frag_color = vec4(inColor, 1.0);
}