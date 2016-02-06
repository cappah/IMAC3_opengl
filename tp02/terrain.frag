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
uniform sampler2D Texture;
uniform vec2 FilterValues;

void main(void)
{

    float filterColor = texture(FilterTexture, In.Texcoord).r;
	if(filterColor >= FilterValue.x && filterColor < FilterValue.y)
	{
		Color = vec4(texture(Texture, In.RepeatedTexcoord).rgb, 1.0);
	}
	else
	{
		discard;
	}

}