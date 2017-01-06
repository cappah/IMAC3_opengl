
uniform mat4 BonesTransform[MAX_BONE_COUNT];
uniform bool UseSkeleton = false;

layout(location = BONE_IDS) in ivec4 BoneIds;
layout(location = BONE_WEIGHTS) in vec4 BoneWeights;


void computeBoneTransform(out mat4 outBoneTransform)
{
    outBoneTransform = mat4(1);
}
