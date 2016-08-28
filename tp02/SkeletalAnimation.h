#pragma once
#include <map>

#include <assimp/anim.h>
#include "Animation.h"
#include "FileHandler.h"

class SkeletalAnimation: public Animation
{
	aiAnimation* m_animation;
	
public:
	SkeletalAnimation();
	SkeletalAnimation(aiAnimation* animation);
	~SkeletalAnimation();

	float getDurationInTick() const;
	float getAnimationTime() const;
	void setIsLooping(bool isLooping);
	std::string getName() const;
	float getTicksPerSecond() const;
	aiNodeAnim* getNodeAnim(const std::string& nodeName) const;
};

//animations attached to a mesh
struct MeshAnimations
{
private:
	std::map<std::string, SkeletalAnimation> m_animations;

public:
	MeshAnimations(const FileHandler::CompletePath& scenePath);
	bool contains(const std::string& name);
	SkeletalAnimation& get(const std::string& name);
};

