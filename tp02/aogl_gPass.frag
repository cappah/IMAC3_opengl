#version 410 core

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define FRAG_COLOR	0

precision highp int;

// inputs : 

layout(location = FRAG_COLOR, index = 0) out vec3 FragColor;

in block
{
        vec2 TexCoord;
        vec3 Position;
        vec3 Normal;
} In;

// Outputs : 

// Write in GL_COLOR_ATTACHMENT0
layout(location = 0 ) out vec4 outColor;
// Write in GL_COLOR_ATTACHMENT1
layout(location = 1) out vec4 outNormal;

// Uniforms : 

uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform float specularPower;

void main()
{

	outColor = vec4( texture(Diffuse, In.TexCoord).rgb, texture(Specular, In.TexCoord).r );

	outNormal = vec4( In.Normal, specularPower/100.0 );
}
