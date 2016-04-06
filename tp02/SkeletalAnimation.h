#pragma once
#include "Animation.h"

#include <assimp/anim.h>

class SkeletalAnimation: public Animation
{
	aiAnimation* m_animation;
	
public:
	SkeletalAnimation(aiAnimation* animation);
	~SkeletalAnimation();

	float getDurationInTick() const;
	float getAnimationTime() const;
	void setIsLooping(bool isLooping);
	std::string getName() const;
	float getTicksPerSecond() const;
	aiNodeAnim* getNodeAnim(const std::string& nodeName) const;
};

