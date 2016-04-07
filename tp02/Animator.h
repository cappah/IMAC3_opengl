#pragma once

#include "Component.h"
#include "SkeletalAnimation.h"
#include "Skeleton.h"

class Animator: public Component
{
private:
	Skeleton* m_skeleton;
	std::string m_currentAnimName;
	std::string m_currentSkeletonName;
	std::map<std::string, SkeletalAnimation*> m_animations;
	bool m_isPlaying;

	//for ui : 
	std::string m_skeletonName;
	std::string m_animationName;

public:
	Animator();
	~Animator();
	
	void setSkeleton(Skeleton* skeleton);
	void addAnimation(SkeletalAnimation* animation);
	void removeAnimation(SkeletalAnimation* animation);
	void updateAnimations(float timeInSecond);

	void play();
	void play(const std::string& animationName);

	virtual void drawUI(Scene & scene) override;
	virtual void eraseFromScene(Scene & scene) override;
	virtual void addToScene(Scene & scene) override;
	virtual void eraseFromEntity(Entity & entity) override;
	virtual void addToEntity(Entity & entity) override;
	virtual Component * clone(Entity * entity) override;

	virtual void save(Json::Value& componentRoot) const override;
	virtual void load(Json::Value& componentRoot) override;
};

