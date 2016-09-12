#include "stdafx.h"

#include "SkeletalAnimation.h"
//forwards :
#include "Application.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

//MESh ANIMATION
MeshAnimations::MeshAnimations(const FileHandler::CompletePath& scenePath)
{
	Assimp::Importer* importer = new Assimp::Importer();
	const aiScene* pScene = importer->ReadFile(scenePath.c_str(), 0); //no flags

	for (int i = 0; i < pScene->mNumAnimations; i++)
	{
		const std::string animName = pScene->mAnimations[i]->mName.data;
		m_animations[animName] = SkeletalAnimation(pScene->mAnimations[i]);
	}
}

bool MeshAnimations::contains(const std::string& name)
{
	return m_animations.find(name) != m_animations.end();
}

SkeletalAnimation& MeshAnimations::get(const std::string& name)
{
	return m_animations[name];
}


//SKELETAL ANIMATION

SkeletalAnimation::SkeletalAnimation()
	: Animation(0.0, true)
	, m_animation(nullptr)
{

}

SkeletalAnimation::SkeletalAnimation(aiAnimation* animation): Animation(0.0, true), m_animation(animation)
{
	float ticksPerSecond = getTicksPerSecond() == 0 ? 25 : getTicksPerSecond();
	m_duration = getDurationInTick() / ticksPerSecond;
}

SkeletalAnimation::~SkeletalAnimation()
{
	delete m_animation;
}

std::string SkeletalAnimation::getName() const
{
	return m_animation->mName.data;
}

float SkeletalAnimation::getTicksPerSecond() const
{
	return m_animation->mTicksPerSecond;
}

aiNodeAnim* SkeletalAnimation::getNodeAnim(const std::string& nodeName) const
{
	for (int i = 0; i < m_animation->mNumChannels; i++) {
		if (m_animation->mChannels[i]->mNodeName.data == nodeName)
			return m_animation->mChannels[i];
	}
	return nullptr;
}

float SkeletalAnimation::getAnimationTime() const
{
	float ticksPerSecond = getTicksPerSecond() != 0 ? getTicksPerSecond() : 25.0f;
	float timeInTicks = getElapsedTime() * ticksPerSecond;
	return fmod(timeInTicks, getDurationInTick()); //note : the fmod is normally not necessary since getElapsedTime already loop the time of the animation.
}

float SkeletalAnimation::getDurationInTick() const
{
	return m_animation->mDuration;
}

void SkeletalAnimation::setIsLooping(bool isLooping)
{
	m_isLooping = isLooping;
}
