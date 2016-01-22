#pragma once

#include "Entity.h"
#include "Gizmo.h"
#include "Application.h"

class Editor
{

private:
	Entity* m_currentSelected;
	Gizmo* m_gizmo;

	Gizmo::GizmoArrowType m_gizmoTranslationDirection;
	glm::vec3 m_gizmoTranslationAnchor;
	bool m_isMovingGizmo;

public:
	Editor(MaterialUnlit* _unlitMaterial);

	void changeCurrentSelected(Entity* entity);

	void renderGizmo(const Camera& camera);//(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)

	void renderUI();

	bool testGizmoIntersection(const Ray& ray);
	void beginMoveGizmo();
	bool isMovingGizmo();
	void endMoveGizmo();
	void moveGizmo(const Ray& ray);

};


