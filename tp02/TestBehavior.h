#pragma once
#include "Behavior.h"



class TestBehavior : public Behavior
{
public:
	BEHAVIOR(TestBehavior)

	TestBehavior();
	virtual ~TestBehavior();

	virtual void start(Scene& scene) override;
	virtual void update(Scene& scene) override;

	// Hérité via Behavior
	virtual void save(Json::Value & entityRoot) const override;
	virtual void load(Json::Value & entityRoot) override;
};


