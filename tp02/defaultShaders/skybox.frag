#version 410 core

#define FRAG_COLOR	0

precision highp int;

// inputs : 

layout(location = FRAG_COLOR, index = 0) out vec4 FragColor;

in block
{
	vec3 TexCoord;
} In;

// Uniforms : 

uniform samplerCube Diffuse;


void main()
{

	FragColor = vec4(texture(Diffuse, In.TexCoord).rgb, 1);

}
