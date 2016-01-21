#include "Gizmo.h"
#include "Editor.h"

Gizmo::Gizmo(MaterialUnlit* _material, Editor* _editor) : target(nullptr), material(_material), mesh(GL_TRIANGLES, (Mesh::USE_INDEX | Mesh::USE_VERTICES)), editor(_editor)
{
	//init mesh
	mesh.triangleIndex = { 0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
	mesh.vertices = { -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
	mesh.initGl();

	meshRenderer.mesh = &mesh;
	meshRenderer.material = material;

	for (int i = 0; i < 3; i++)
	{
		collider[i].setVisual(&meshRenderer);
	}

	collider[0].applyScale(glm::scale(glm::mat4(1), glm::vec3(2.f, 0.2f, 0.2f))); // x
	collider[1].applyScale(glm::scale(glm::mat4(1), glm::vec3(0.2f, 2.f, 0.2f))); // y
	collider[2].applyScale(glm::scale(glm::mat4(1), glm::vec3(0.2f, 0.2f, 2.f))); // z

	collider[0].applyTranslation(glm::translate(glm::mat4(1), glm::vec3(1.f, 0.f, 0.f))); // x
	collider[1].applyTranslation(glm::translate(glm::mat4(1), glm::vec3(0.f, 1.f, 0.f))); // y
	collider[2].applyTranslation(glm::translate(glm::mat4(1), glm::vec3(0.f, 0.f, 1.f))); // z
}

void Gizmo::setTarget(Entity* entity)
{
	//set target : 
	target = entity;

	//update collider position : 
	for (int i = 0; i < 3; i++)
	{
		collider[i].applyTranslation(entity->modelMatrix);

		collider[0].appendTranslation(glm::translate(glm::mat4(1), glm::vec3(1.f, 0.f, 0.f))); // x
		collider[1].appendTranslation(glm::translate(glm::mat4(1), glm::vec3(0.f, 1.f, 0.f))); // y
		collider[2].appendTranslation(glm::translate(glm::mat4(1), glm::vec3(0.f, 0.f, 1.f))); // z
	}

	editor->changeCurrentSelected(entity);
}

void Gizmo::render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{
	glm::mat4 modelMatrix(1);
	glm::mat4 translation(1);
	if (target != nullptr)
	{
		translation = glm::translate(glm::mat4(1), glm::vec3(target->modelMatrix[3][0], target->modelMatrix[3][1], target->modelMatrix[3][2]));
	}

	MaterialUnlit* material = static_cast<MaterialUnlit*>(meshRenderer.material);

	material->use();
	material->setUniform_normalMatrix(glm::mat4(1));

	//x : 
	modelMatrix = translation * glm::translate(glm::mat4(1), glm::vec3(1.f, 0.f, 0.f)) * glm::scale(glm::mat4(1), glm::vec3(2.f, 0.2f, 0.2f));
	material->setUniform_MVP(projectionMatrix * viewMatrix * modelMatrix);
	material->setUniform_color(glm::vec3(1, 0, 0));
	mesh.draw();

	//y : 
	material->setUniform_color(glm::vec3(0, 1, 0));
	modelMatrix = translation * glm::translate(glm::mat4(1), glm::vec3(0.f, 1.f, 0.f)) * glm::scale(glm::mat4(1), glm::vec3(0.2f, 2.f, 0.2f));
	material->setUniform_MVP(projectionMatrix * viewMatrix * modelMatrix);
	mesh.draw();

	//z : 
	material->setUniform_color(glm::vec3(0, 0, 1));
	modelMatrix = translation * glm::translate(glm::mat4(1), glm::vec3(0.f, 0.f, 1.f)) * glm::scale(glm::mat4(1), glm::vec3(0.2f, 0.2f, 2.f));
	material->setUniform_MVP(projectionMatrix * viewMatrix * modelMatrix);
	mesh.draw();
}