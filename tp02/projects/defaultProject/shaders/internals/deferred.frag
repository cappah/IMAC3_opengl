#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define TANGENT		3
#define FRAG_COLOR	0

// inputs : 
in block
{
        vec2 TexCoord;
        vec3 Position;
        mat3 TBN;
} In;

// Outputs : 

// Write in GL_COLOR_ATTACHMENT0
layout(location = 0 ) out vec4 outColor;
// Write in GL_COLOR_ATTACHMENT1
layout(location = 1) out vec4 outNormal;
// Write in GL_COLOR_ATTACHMENT2
layout(location = 2) out vec4 outHighValues;
// Write in GL_COLOR_ATTACHMENT3
layout(location = 3) out vec4 outPositions;

void main()
{
        vec4 paramDiffuse;
        vec3 paramEmissive;
        vec3 paramNormals;
        float paramSpecular;
        float paramSpecularPower;

        computeShaderParameters(paramDiffuse, paramNormals, paramSpecular, paramSpecularPower, paramEmissive);


        outColor = vec4( paramDiffuse, paramSpecular );
        outHighValues = vec4(paramEmissive, 1.0);

        vec3 bumpNormal = paramNormals;
        bumpNormal = normalize(bumpNormal * 2.0 - 1.0);
        bumpNormal = normalize(In.TBN * bumpNormal);
        outNormal = vec4( bumpNormal*0.5+0.5, paramSpecularPower/100.0 );

        outPositions = vec4(In.Position, 1.0);
}
