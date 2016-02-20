#version 410 core

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define TANGENT		3
#define FRAG_COLOR	0

precision highp int;

// inputs : 

layout(location = FRAG_COLOR, index = 0) out vec3 FragColor;

in block
{
        vec2 TexCoord;
        vec3 Position;
        //vec3 Normal;
		mat3 TBN;
} In;

// Outputs : 

// Write in GL_COLOR_ATTACHMENT0
layout(location = 0 ) out vec4 outColor;
// Write in GL_COLOR_ATTACHMENT1
layout(location = 1) out vec4 outNormal;

// Uniforms : 

uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform sampler2D Bump;
uniform float specularPower;

void main()
{

	outColor = vec4( texture(Diffuse, In.TexCoord).rgb, 1);// texture(Specular, In.TexCoord).r );

	vec3 bumpNormal = texture(Bump, In.TexCoord).rgb;
	bumpNormal = normalize(bumpNormal * 2.0 - 1.0);
	bumpNormal = normalize(In.TBN * bumpNormal);

	outNormal = vec4( bumpNormal*0.5+0.5, specularPower/100.0 );
}
