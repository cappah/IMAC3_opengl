#pragma once

#include "Entity.h"
#include "Gizmo.h"
#include "Application.h"
#include "Factories.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

//forward
class Scene;

class Editor
{

private:
	std::vector<Entity*> m_currentSelected;
	Gizmo* m_gizmo;

	Gizmo::GizmoArrowType m_gizmoTranslationDirection;
	glm::vec3 m_gizmoTranslationAnchor;
	bool m_isMovingGizmo;

	bool m_isUIVisible;
	bool m_isGizmoVisible;

public:
	Editor(MaterialUnlit* _unlitMaterial);

	void changeCurrentSelected(Entity* entity);
	void changeCurrentSelected(std::vector<Entity*> entities);
	void addCurrentSelected(Entity* entity);
	void removeCurrentSelected(Entity* entity);
	void toggleCurrentSelected(Entity* entity);

	void renderGizmo(const Camera& camera);//(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)

	void renderUI(Scene& scene);

	bool testGizmoIntersection(const Ray& ray);
	void beginMoveGizmo();
	bool isMovingGizmo();
	void endMoveGizmo();
	void moveGizmo(const Ray& ray);

	Entity* duplicateSelected();

	void deleteSelected(Scene& scene);

	void toggleUIVisibility();
	void toggleGizmoVisibility();
	void toggleDebugVisibility(Scene& scene);

};


