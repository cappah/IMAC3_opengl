#version 410 core

// Inputs : 
in block
{
    vec2 Texcoord;
	vec2 RepeatedTexcoord;
} In; 



// Outputs : 
layout(location = 0, index = 0) out vec4  Color;

//Uniforms : 
uniform sampler2D FilterTexture;
uniform sampler2D Diffuse;
uniform vec2 FilterValues;

void main(void)
{

    float filterColor = texture(FilterTexture, In.Texcoord).r;
	if(filterColor >= (FilterValues.x) && filterColor < (FilterValues.y))
	{
		Color = vec4(texture(Diffuse, In.RepeatedTexcoord).rgb, 1.0);
	}
	else
	{
		discard;
	}
}