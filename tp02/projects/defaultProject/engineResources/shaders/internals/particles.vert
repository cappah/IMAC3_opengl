#define POSITIONS 0
#define NORMALS 1
#define UVS 2
#define TRANSLATIONS 3
#define COLORS 4
#define SIZES 5

layout(location = POSITIONS) in vec3 Position;
layout(location = NORMALS) in vec3 Normal;
layout(location = UVS) in vec2 TexCoord;
layout(location = TRANSLATIONS) in vec3 Translation;
layout(location = COLORS) in vec4 Color;
layout(location = SIZES) in vec2 Size;

uniform vec3 CameraRight;
uniform vec3 CameraUp;
uniform mat4 VP;

uniform vec4 ClipPlane;

out block
{
        vec2 TexCoord;
        vec3 Position;
        mat3 TBN;
        vec2 NormalizedPos2D;
} Out;

void main()
{
        Out.Color = Color;

        vec3 tangent = normalize(CameraRight);
        //calculate TBN matrix :
        vec3 T = normalize( vec3(vec4(tangent, 0.0)) );
        vec3 N = normalize( vec3(vec4(Normal, 0.0)) );
        vec3 B = -cross(T, N);
        Out.TBN = mat3(B, T, N);

        Out.TexCoord = TexCoord;// * TextureRepetition;
        vec3 posWorldSpace = Translation + CameraRight * Position.x * Size.x + CameraUp * Position.z * Size.y;
        Out.Position = posWorldSpace; // To view space ?

        gl_ClipDistance[0] = dot(vec4(posWorldSpace, 1.0), ClipPlane);

        gl_Position = VP * vec4(posWorldSpace, 1.0);
        Out.NormalizedPos2D = gl_Position.xy;
        Out.NormalizedPos2D /= gl_Position.w;
        Out.NormalizedPos2D += 1.0f;
        Out.NormalizedPos2D *= 0.5f;
}
