#version 420
//gbuffer pass 

//data for this model
layout(location =0) in vec3 inPos;
layout(location =1) in vec3 inColor;
layout(location =2) in vec3 inNormal;
layout(location =3) in vec2 inUV;


//albedo textures
uniform sampler2D s_Diffuse;
uniform sampler2D s_Diffuse2;
uniform sampler2D s_Specular;
uniform float u_textureMix;

// multi render target 
//we can render color to all of these
layout (location = 0 ) out vec4 outColors;
layout (location = 1 ) out vec3 outNormals; 
layout (location = 2 ) out vec3 outSpecs;
layout (location = 3 ) out vec3 outPositions;

void main()
{

    //get the albedo form diffuse/albedo map
    vec4 textureColor1 = texture(s_Diffuse,inUV);
    vec4 textureColor2 = texture(s_Diffuse2,inUV);
    vec4 textureColor = mix(textureColor1,textureColor2, u_textureMix);

    //outputs texture color
    outColors = textureColor;

    //outputs normals color
    //[-1, 1] -> inNormals range
    // [0, 1] -> OutNormals range
    //so we convert
    outNormals = (normalize(inNormal)*0.5)+0.5;

//outputs specular  colors
outSpecs=texture(s_Specular,inUV).rgb;

//output viewspace positions
outPositions = inPos;



}




