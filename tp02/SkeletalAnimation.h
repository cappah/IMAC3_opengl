#pragma once
#include "Animation.h"

#include <assimp/anim.h>

class SkeletalAnimation: public Animation
{
	aiAnimation* m_animation;
	
public:
	SkeletalAnimation(aiAnimation* animation);
	~SkeletalAnimation();

	std::string getName() const;
	float getTicksPerSecond() const;
	float getDuration() const;
	aiNodeAnim* getNodeAnim(const std::string& nodeName) const;
};

