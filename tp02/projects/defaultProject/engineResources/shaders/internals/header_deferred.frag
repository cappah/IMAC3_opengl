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
        vec2 NormalizedPos2D;
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
