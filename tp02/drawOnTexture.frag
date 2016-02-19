#version 410 core

// Inputs : 
in block
{
    vec2 Texcoord;
	vec3 Position
} In; 

// Outputs : 
layout(location = 0) out vec4 outColor;

//Uniforms : 
uniform sampler2D Texture;
uniform vec4 DrawColor
uniform vec3 DrawPosition
uniform float DrawRadius

void main(void)
{
	
    vec4 tmpColor = Color;
	float d = length(In.Position - DrawPosition);
	tmpColor = tmpColor / (d*d);

    outColor = tmpColor;
}