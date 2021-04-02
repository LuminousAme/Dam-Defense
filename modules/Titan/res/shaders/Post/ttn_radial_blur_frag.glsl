#version 420 

//take in the uvs from the vertex shader
layout(location = 0) in vec2 inUV;

//pass on the fragment color
out vec4 frag_color; 

//take in the screen texture from a framebuffer
layout(binding = 0) uniform sampler2D s_SourceImage;

//radius for radial blur
uniform float u_Radius=1.0f; 

//strength of blur
uniform float u_Strength = 2.1f;

//the samples for the blur
uniform float u_Samples[10];

//reference: https://github.com/geoo993/ComputerGraphicsWithOpenGL/blob/master/src/resources/shaders/RadialBlurShader.frag
// kinda helpful explaination: https://www.shadertoy.com/view/XsKGRW
// link: https://www.shadertoy.com/view/MdG3RD
void main() {
    
    //should use 
	 float samples[10];
    samples[0] = -0.8;
    samples[1] = -0.05;
    samples[2] = -0.03;
    samples[3] = -0.02;
    samples[4] = -0.1;
    samples[5] =  0.1;
    samples[6] =  0.02;
    samples[7] =  0.03;
    samples[8] =  0.05;
    samples[9] =  0.08;

    //get the position that's behind the texture uv (for the rays/ blur direction)
    vec2 dir = 0.5f - inUV; 
    //pythag theorem to get hypotnesue
    float dist = sqrt(dir.x*dir.x + dir.y*dir.y); 
    dir = dir/dist; 

    //get screen texture
    vec4 color = texture2D(s_SourceImage,inUV); 
    vec4 sum = color;

   // Take a texture sample, move a little in the direction of the radial direction vector (dir) then take another, slightly more weighted,
    // sample, add it to the total, then repeat the process until done. (
    for (int i = 0; i < 10; i++){
        sum += texture2D( s_SourceImage, inUV + dir * samples[i] * u_Radius );

     }

    sum *= 1.0f/11.0f;
    float t = dist * u_Strength;
    t = clamp( t ,0.0f,1.0f);

   // color *= (1.f - dot(dir, dir)*.75f);

    vec4 result = mix(color, sum, t );

    frag_color = vec4(result.rgb, 1.0);
}