#version 410

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

uniform mat4 u_modelView;
uniform mat4 u_vp;

in Vertex 
{
	float scale;
	vec4 color;
} vertex[];


layout (location = 0) out vec2 outUv;
layout(location = 1) out vec4 outColor;

void main() {
	//get the right and up vectors 
	 vec3 right = vec3(u_modelView[0][0], 
						u_modelView[1][0], 
						u_modelView[2][0]);

	vec3 up = vec3(u_modelView[0][1], 
					u_modelView[1][1], 
					u_modelView[2][1]);

	vec3 point = gl_in[0].gl_Position.xyz;

	//construct the first vertex
	vec3 vertexA = point - (right + up) * vertex[0].scale;
	gl_Position = u_vp * vec4(vertexA, 1.0);
	outUv = vec2(0.0, 0.0);
	outColor = vertex[0].color;
	EmitVertex();

	//construct the fourth vertex
	vec3 vertexD = point + (right - up) * vertex[0].scale;
	gl_Position = u_vp * vec4(vertexD, 1.0);
	outUv = vec2(1.0, 0.0);
	outColor = vertex[0].color;
	EmitVertex();

	//construct the second vertex
	vec3 vertexB = point - (right - up) * vertex[0].scale;
	gl_Position = u_vp * vec4(vertexB, 1.0);
	outUv = vec2(0.0, 1.0);
	outColor = vertex[0].color;
	EmitVertex();

	//construct the third vertex
	vec3 vertexC = point + (right + up) * vertex[0].scale;
	gl_Position = u_vp * vec4(vertexC, 1.0);
	outUv = vec2(1.0, 1.0);
	outColor = vertex[0].color;
	EmitVertex();

	EndPrimitive();
}