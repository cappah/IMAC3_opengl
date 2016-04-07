#pragma once

#include <vector>
#include <map>

#include "glm/glm.hpp"
#include "glm/common.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "SkeletalAnimation.h"

static const unsigned int MAX_BONE_DATA_PER_VERTEX = 4;

struct BoneInfo {
	//TODO
};

struct VertexBoneData {
	unsigned int ids[MAX_BONE_DATA_PER_VERTEX];
	float weights[MAX_BONE_DATA_PER_VERTEX];

	inline VertexBoneData() {
		for (int i = 0; i < MAX_BONE_DATA_PER_VERTEX; i++) {
			ids[i] = 0;
			weights[i] = 0;
		}
	}
};

class Skeleton
{
	unsigned int m_boneCount;
	std::map<std::string, unsigned int> m_boneMapping;
	std::vector<VertexBoneData> m_boneDatas;
	std::vector<aiMatrix4x4> m_bonesOffset;
	std::vector<glm::mat4> m_bonesTransform;
	const aiNode* m_rootNode;
	aiMatrix4x4 m_globalInverseTransform;

public:
	Skeleton(const aiMesh* pMesh, const aiNode* rootNode, unsigned int firstVertexId);
	~Skeleton();

	unsigned int getBoneCount() const;
	const std::vector<aiMatrix4x4>& getBonesOffset() const;
	const std::vector<glm::mat4>& getBonesTransform() const;
	const glm::mat4& getBoneTransform(int i) const;
	const std::vector<VertexBoneData>& getBoneDatas() const;

	void playAnimationStep(float timeInSecond, const SkeletalAnimation& animation);
	void loadBones(const aiMesh* pMesh, unsigned int firstVertexId);

private:
	void readNodeHierarchy(float animationTime, const SkeletalAnimation& animation, const aiNode* node, const aiMatrix4x4& parentTransform);

	unsigned int findPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int findRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int findScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	void computeInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void computeInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void computeInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
};

