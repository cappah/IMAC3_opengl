#include "SkeletalAnimation.h"



SkeletalAnimation::SkeletalAnimation(aiAnimation* animation): m_animation(animation)
{
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

float SkeletalAnimation::getDuration() const
{
	return m_animation->mDuration;
}

aiNodeAnim* SkeletalAnimation::getNodeAnim(const std::string& nodeName) const
{
	for (int i = 0; i < m_animation->mNumChannels; i++) {
		if (m_animation->mChannels[i]->mNodeName.data == nodeName)
			return m_animation->mChannels[i];
	}
	return nullptr;
}
