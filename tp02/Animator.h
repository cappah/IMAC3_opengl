#pragma once

#include "Component.h"
#include "SkeletalAnimation.h"
#include "Skeleton.h"
#include "ResourcePointer.h"

class Animator: public Component
{
private:
	FileHandler::CompletePath m_skeletonPath; //TODO : J'aime pas �a, faire en sorte que le skeleton soit une sous-resource ce serai mieux
	Skeleton* m_skeleton;
	int m_currentAnimIdx;
	std::vector<ResourcePtr<SkeletalAnimation>> m_skeletonAnimations;
	bool m_isPlaying;

	//for ui : 
	std::string m_skeletonName;
	std::string m_animationName;

public:
	Animator();
	~Animator();
	
	void setSkeleton(Skeleton* skeleton);
	void addAnimation(ResourcePtr<SkeletalAnimation> animation);
	void removeAnimation(SkeletalAnimation* animation);
	void updateAnimations(float timeInSecond);

	void play();
	void play(const std::string& animationName);
	void play(int animationIdx);

	virtual void drawUI(Scene & scene) override;
	virtual void eraseFromScene(Scene & scene) override;
	virtual void addToScene(Scene & scene) override;
	virtual void eraseFromEntity(Entity & entity) override;
	virtual void addToEntity(Entity & entity) override;
	virtual Component * clone(Entity * entity) override;

	virtual void save(Json::Value& componentRoot) const override;
	virtual void load(Json::Value& componentRoot) override;
};

