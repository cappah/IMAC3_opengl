#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Component.h"

class PathPoint : public Component
{
private:
	int m_pathId; //the path this point belong to
	int m_pointIdx; //the index in the current path

public:
	PathPoint();
	~PathPoint();

	glm::vec3 getPosition();
	int getPointIdx() const;
	int getPathId() const;

	void drawUI();

	// Hérité via Component
	virtual void eraseFromScene(Scene & scene) override;
	virtual void addToScene(Scene & scene) override;
	virtual Component * clone(Entity * entity) override;
};