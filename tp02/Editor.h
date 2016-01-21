#pragma once

#include "Entity.h"
#include "Gizmo.h"
#include "Application.h"

class Editor
{

private:
	Entity* currentSelected;
	Gizmo* gizmo;

public:
	Editor(MaterialUnlit* _unlitMaterial);

	void changeCurrentSelected(Entity* entity);

	void renderGizmo(const Camera& camera);//(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)

	void renderUI();

};


