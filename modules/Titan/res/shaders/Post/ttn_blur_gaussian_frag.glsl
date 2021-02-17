#version 420

layout(location = 0) in vec2 inUV;

layout(binding = 0) uniform sampler2D s_screenTex; //Source s_screenTex

out vec4 frag_color;
uniform float u_Blur;

uniform bool u_Horizontal;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
//uniform float weight[5];

//CODE FROM: https://learnopengl.com/Advanced-Lighting/Bloom
void main() 
{
	vec2 TexelSize = 1.0 / textureSize(s_screenTex, 0); // gets size of single texel

  //  float weight[5] = float[] (0.227027, 0.1945946+(u_Blur/2.f), 0.1216216+(u_Blur/2.f), 0.054054+(u_Blur/2.f), 0.016216+(u_Blur/2.f));

    vec3 result = texture(s_screenTex, inUV).rgb * weight[0]; // current fragment's contribution

    //Sample pixels  horizontally and vertically
	if(u_Horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(s_screenTex, inUV + vec2(TexelSize.x * i, 0.0)).rgb * weight[i];
            result += texture(s_screenTex, inUV - vec2(TexelSize.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {//vertical
        for(int i = 1; i < 5; ++i)
        {
            result += texture(s_screenTex, inUV + vec2(0.0, TexelSize.y * i)).rgb * weight[i];
            result += texture(s_screenTex, inUV - vec2(0.0, TexelSize.y * i)).rgb * weight[i];
        }
    }

    frag_color = vec4(result, 1.0);


}