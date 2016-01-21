#pragma once

#include "Mesh.h"
#include "MeshRenderer.h"
#include "Materials.h"
#include "Entity.h"

//forward
class Editor;

class Gizmo
{

private:
	Editor* editor;
	Entity* target;

	Mesh mesh;
	MaterialUnlit* material;
	MeshRenderer meshRenderer;
	Collider collider[3];

public:
	Gizmo(MaterialUnlit* _material = nullptr, Editor* _editor = nullptr);

	void setTarget(Entity* entity);

	void render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);

};
