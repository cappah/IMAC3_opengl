#include "Entity.h"

Entity::Entity() : modelMatrix(glm::mat4(1)), meshRenderer(nullptr), collider(nullptr)
{

}

void Entity::applyTransform()
{
	if (collider != nullptr)
		collider->applyTransform(modelMatrix);
}

void Entity::drawUI()
{
	ImGui::Begin("entity");
	ImGui::InputText(name.c_str(), &name[0], name.size());
	ImGui::End();
	//...TODO
}