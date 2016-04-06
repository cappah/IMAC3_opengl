#include "Skeleton.h"
//forwards : 
#include "Utils.h"



Skeleton::Skeleton(const aiMesh* pMesh, const aiNode* rootNode, unsigned int firstVertexId): m_rootNode(rootNode)
{
	m_globalInverseTransform = rootNode->mTransformation;
	m_globalInverseTransform.Inverse();

	loadBones(pMesh, firstVertexId);
}


Skeleton::~Skeleton()
{
}

unsigned int Skeleton::getBoneCount() const
{
	return m_boneCount;
}

const std::vector<aiMatrix4x4>& Skeleton::getBonesOffset() const
{
	return m_bonesOffset;
}

const std::vector<glm::mat4>& Skeleton::getBonesTransform() const
{
	return m_bonesTransform;
}

const std::vector<VertexBoneData>& Skeleton::getBoneDatas() const
{
	return m_boneDatas;
}

void Skeleton::playAnimationStep(float timeInSecond, const SkeletalAnimation& animation)
{
	float ticksPerSecond = animation.getTicksPerSecond() != 0 ? animation.getTicksPerSecond() : 25.0f;
	float timeInTicks = timeInSecond * ticksPerSecond;
	float animationTime = fmod(timeInTicks, animation.getDuration());

	aiMatrix4x4 identity;
	readNodeHierarchy(animationTime, animation, m_rootNode, identity );
}

void Skeleton::loadBones(const aiMesh* pMesh, unsigned int firstVertexId)
{
	m_bonesTransform.resize(pMesh->mNumBones);
	m_bonesOffset.resize(pMesh->mNumBones);

	for (unsigned int i = 0; i < pMesh->mNumBones; i++) {
		std::string boneName = pMesh->mBones[i]->mName.data;
		unsigned int boneIndex = 0;

		if (m_boneMapping.find(boneName) == m_boneMapping.end()) {
			boneIndex = m_boneCount++;

			m_boneMapping[boneName] = boneIndex;
			m_bonesOffset[boneIndex] = pMesh->mBones[i]->mOffsetMatrix;
		}
		else {
			boneIndex = m_boneMapping[boneName];
			m_bonesOffset[boneIndex] = pMesh->mBones[i]->mOffsetMatrix;
		}

		for (unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
			float weight = pMesh->mBones[i]->mWeights[j].mWeight;
			if (weight == 0)
				continue;

			unsigned int vertexId = pMesh->mBones[i]->mWeights[j].mVertexId + firstVertexId;

			for (int k = 0; k < MAX_BONE_DATA_PER_VERTEX; k++) {
				if (m_boneDatas[vertexId].weights[k] == 0) {
					m_boneDatas[vertexId].ids[k] = boneIndex;
					m_boneDatas[vertexId].weights[k] = weight;
					break;
				}
			}
		}
	}
}

unsigned int Skeleton::findPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


unsigned int Skeleton::findRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


unsigned int Skeleton::findScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


void Skeleton::computeInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned int PositionIndex = findPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void Skeleton::computeInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned int RotationIndex = findRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}


void Skeleton::computeInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = findScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void Skeleton::readNodeHierarchy(float animationTime, const SkeletalAnimation& animation, const aiNode* node, const aiMatrix4x4& parentTransform)
{
	std::string nodeName = node->mName.data;
	aiNodeAnim* nodeAnim = animation.getNodeAnim(nodeName);
	aiMatrix4x4 nodeTransformation = node->mTransformation;

	if (nodeAnim) {

		aiVector3D scaling;
		computeInterpolatedScaling(scaling, animationTime, nodeAnim);
		aiMatrix4x4 scalingM;
		aiMatrix4x4::Scaling(scaling, scalingM);

		aiQuaternion rotationQ;
		computeInterpolatedRotation(rotationQ, animationTime, nodeAnim);
		aiMatrix4x4 rotationM = aiMatrix4x4(rotationQ.GetMatrix());

		aiVector3D translation;
		computeInterpolatedPosition(translation, animationTime, nodeAnim);
		aiMatrix4x4 translationM;
		aiMatrix4x4::Translation(translation, translationM);

		nodeTransformation = translationM * rotationM * scalingM;
	}

	aiMatrix4x4 globalTransformation = parentTransform * nodeTransformation;

	if (m_boneMapping.find(nodeName) != m_boneMapping.end()) {

		unsigned int boneIndex = m_boneMapping[nodeName];
		assimpMat4ToglmMat4( m_globalInverseTransform * globalTransformation * m_bonesOffset[boneIndex], m_bonesTransform[boneIndex]);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {

		readNodeHierarchy(animationTime, animation, node->mChildren[i], globalTransformation);
	}
}
