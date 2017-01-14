#version 410 core


precision highp float;
precision highp int;

uniform vec3 Translation;
uniform vec2 Scale;
uniform vec3 CameraRight;
uniform vec3 CameraUp;

uniform vec4 ClipPlane;

uniform mat4 MVP;
//uniform mat4 NormalMatrix;
//uniform vec2 TextureRepetition;

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;
layout(location = 3) in vec3 Tangent;


out block
{
        vec2 TexCoord;
        vec3 Position;
        mat3 TBN;
        vec2 NormalizedPos2D;
} Out;

void main()
{
        vec3 posWorldSpace = Translation + CameraRight * Position.x * Scale.x + CameraUp * Position.z * Scale.y;

        vec3 tangent = normalize(CameraRight);
        //calculate TBN matrix :
        vec3 T = normalize( vec3(vec4(tangent, 0.0)) );
        vec3 N = normalize( vec3(vec4(Normal, 0.0)) );
        vec3 B = -cross(T, N);
        Out.TBN = mat3(B, T, N);

        Out.TexCoord = TexCoord;// * TextureRepetition;
        Out.Position = posWorldSpace; // passer en view space ?
        gl_Position = MVP * vec4(posWorldSpace, 1.0);

        gl_ClipDistance[0] = dot(vec4(posWorldSpace, 1.0), ClipPlane);

        Out.NormalizedPos2D = gl_Position.xy;
        Out.NormalizedPos2D /= gl_Position.w;
        Out.NormalizedPos2D += 1.0f;
        Out.NormalizedPos2D *= 0.5f;
}
