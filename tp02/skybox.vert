#version 410 core

#define POSITION	0
#define TEXCOORD	2
#define FRAG_COLOR	0

precision highp float;
precision highp int;

uniform mat4 MVP;
uniform mat4 NormalMatrix;

layout(location = POSITION) in vec3 Position;
layout(location = TEXCOORD) in vec2 TexCoord;


out block
{
	vec3 TexCoord; 
} Out;

void main()
{	
	vec4 pos = MVP * vec4(Position, 1.0);
    gl_Position = pos.xyww;
    Out.TexCoord = Position;
	
}
