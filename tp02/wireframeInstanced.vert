#version 410 core

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define TRANSLATION	3

precision highp float;
precision highp int;

layout(location = POSITION) in vec3 Position;
layout(location = NORMAL) in vec3 Normal;
layout(location = TEXCOORD) in vec2 TexCoord;
layout(location = TRANSLATION) in vec3 Translation;

uniform float Color;
uniform mat4 VP;

out block
{
	vec2 TexCoord; 
	vec3 Position;
	vec3 Normal;
} Out;

void main()
{	
	Out.TexCoord = TexCoord;
	Out.Position = Position;
	Out.Normal =  Normal; //normalize( vec3(NormalMatrix * vec4(Normal, 0)) );

	gl_Position = VP * vec4(Position + Translation, 1);
}
