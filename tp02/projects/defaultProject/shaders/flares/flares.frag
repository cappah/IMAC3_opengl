#version 410 core

// Inputs : 
in block
{
    vec2 Texcoord;
    flat int FlareIdx;
} In; 

// Outputs : 
layout(location = 0) out vec4  outColor;

//Uniforms : 
uniform sampler2D MainFlare;
uniform sampler2D TrailFlare;

void main(void)
{
    vec3 color = In.FlareIdx == 0 ? texture(MainFlare, In.Texcoord).rgb : texture(TrailFlare, In.Texcoord).rgb;
    outColor = vec4(color, 1.0);
}
