#version 410 core

#define POSITION	0
#define FRAG_COLOR	0

precision highp float;
precision highp int;

uniform mat4 MVP;
uniform mat4 NormalMatrix;

layout(location = POSITION) in vec3 Position;


void main()
{	
	gl_Position = MVP * vec4(Position,1);
}
