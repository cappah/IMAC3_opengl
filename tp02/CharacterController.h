#pragma once

#include "Component.h"

class CharacterController: public Component
{
public:
	CharacterController();
	~CharacterController();

	// Hérité via Component
	virtual void drawUI(Scene & scene) override;
	virtual void eraseFromScene(Scene & scene) override;
	virtual void addToScene(Scene & scene) override;
	virtual void eraseFromEntity(Entity & entity) override;
	virtual void addToEntity(Entity & entity) override;
	virtual Component * clone(Entity * entity) override;
};

