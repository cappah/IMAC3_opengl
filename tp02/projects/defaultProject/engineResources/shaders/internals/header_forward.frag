
layout(location = 0)out vec4 Color;
layout(location = 1)out vec4 HighValues;

//inputs :
in block
{
        vec2 TexCoord;
        vec3 Position;
        mat3 TBN;
        vec2 NormalizedPos2D;
} In;
