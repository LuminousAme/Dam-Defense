#version 420
//mesh data from c++ program
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

//mesh data to pass to the frag shader
layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;
layout(location = 3) out vec4 outClipSpace;

//model, view, projection matrix
uniform mat4 MVP;
//model matrix only
uniform mat4 Model;
//normal matrix
uniform mat3 NormalMat;

//consistent controls for all waves
uniform float time;
uniform float u_Q;
uniform int u_numOfWaves;
uniform float u_speed;

//per wave controls
uniform float u_amplitude[16];
uniform float u_frequency[16];
uniform vec3 u_direction[16];

vec3 CalcWave(vec3 pos, int iteration);
vec3 CalcWaveNormal(vec3 pos, int iteration);

void main() {
	//pass data onto the frag shader
	outUV = inUV;

	//calculate the position displacments and normals of the wave
	vec3 posSum = vec3(0.0);
	vec3 normalSum = vec3(0.0);
	for(int i = 0; i < u_numOfWaves; i++) {
		posSum += CalcWave(inPos, i);
		normalSum += CalcWaveNormal(inPos, i);
	}

	//set the positions and the normal for this vertex
	vec3 vertexPos = vec3(inPos.x, posSum.y, inPos.z);
	vec3 vertexNormal = vec3(-1.0 * normalSum.x, 1.0 - normalSum.y, -1.0 * normalSum.z);

	//pass that data onto the fragment shader
	outPos = (Model * vec4(vertexPos, 1.0)).xyz;
	outNormal = NormalMat * vertexNormal;
	
	//and store it as the position
	outClipSpace = MVP * vec4(vertexPos, 1.0);
	gl_Position = outClipSpace;
}

//function that calculates the ammount of a wave that should be added
vec3 CalcWave(vec3 pos, int iteration) {
	//calculate details of the wave
	float Wi = u_frequency[iteration];
	float Ai = u_amplitude[iteration];
	float Lamdai = u_speed * Wi;
	float Qi = u_Q / (Wi * Ai * u_numOfWaves);
	vec2 dir = vec2(u_direction[iteration].x, u_direction[iteration].z);

	//horizontal displacement
	float deltaX = Qi * Ai * dir.x + cos(Wi * dot(dir, vec2(pos.x, pos.z)) + Lamdai * time);
	float deltaZ = Qi * Ai * dir.y + cos(Wi * dot(dir, vec2(pos.x, pos.z)) + Lamdai * time);
	//vertical displacment
	float deltaY = Ai * sin(Wi * dot(dir, vec2(pos.x, pos.z)) + Lamdai * time);

	return vec3(deltaX, deltaY, deltaZ);
}

//function that calculates the ammount of a wave's normal that should be added
vec3 CalcWaveNormal(vec3 pos, int iteration) {
	//calculate details of the wave
	float Wi = u_frequency[iteration];
	float Ai = u_amplitude[iteration];
	float Lamdai = u_speed * Wi;
	float Qi = u_Q / (Wi * Ai * u_numOfWaves);
	vec2 dir = vec2(u_direction[iteration].x, u_direction[iteration].z);

	float WA = Wi * Ai; 
	float S = sin(Wi * dot(dir, vec2(pos.x, pos.z) + Lamdai * time));
	float C = cos(Wi * dot(dir, vec2(pos.x, pos.z) + Lamdai * time));

	//horizontal 
	float normalX = dir.x * WA * C;
	float normalZ = dir.y * WA * C;
	//vertical
	float normalY = Qi * WA * S;

	return vec3(normalX, normalY, normalZ);
}