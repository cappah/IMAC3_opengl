#pragma once

#include "BasicColliders.h"

class IDrawable
{
public:
	virtual const AABB& getVisualBoundingBox() const = 0;
	virtual void draw() const = 0;
	virtual const glm::mat4& getModelMatrix() const = 0;
	virtual bool castShadows() const = 0;
};