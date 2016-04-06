#include "SkeletalAnimation.h"
//forwards :
#include "Application.h"


SkeletalAnimation::SkeletalAnimation(aiAnimation* animation): Animation(), m_animation(animation)
{
	m_duration = getTicksPerSecond() * getDuration();
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
	return fmod(timeInTicks, getDuration()); //note : the fmod is normally not necessary since getElapsedTime already loop the time of the animation.
}

float SkeletalAnimation::getDurationInTick() const
{
	return m_animation->mDuration;
}

void SkeletalAnimation::setIsLooping(bool isLooping)
{
	m_isLooping = isLooping;
}
