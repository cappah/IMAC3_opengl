#pragma once

#include "Mesh.h"
#include "MeshRenderer.h"
#include "Materials.h"
#include "Entity.h"
#include "Ray.h"

//forward
class Editor;

class Gizmo
{
public:
	enum GizmoArrowType{ GIZMO_ARROW_NONE = -1, GIZMO_ARROW_X = 0, GIZMO_ARROW_Y = 1, GIZMO_ARROW_Z = 2 };

private:
	Editor* editor;
	std::vector<Entity*> targets;
	glm::vec3 position;

	Mesh mesh;
	MaterialUnlit* material;
	MeshRenderer meshRenderer;
	BoxCollider collider[3];

public:
	Gizmo(MaterialUnlit* _material = nullptr, Editor* _editor = nullptr);

	void setTarget(Entity* entity);
	void setTargets(std::vector<Entity*> entity);

	void render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);

	GizmoArrowType checkIntersection(const Ray& ray, glm::vec3& intersectionPoint);

	void translate(const glm::vec3& t);
	void setTranslation(const glm::vec3& t);

};
