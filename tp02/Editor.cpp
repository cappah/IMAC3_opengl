#include "Editor.h"

Editor::Editor(MaterialUnlit* _unlitMaterial) : currentSelected(nullptr)
{
	gizmo = new Gizmo(_unlitMaterial, this);
}

void Editor::changeCurrentSelected(Entity* entity)
{
	currentSelected = entity;
	
	if(gizmo != nullptr && entity != nullptr)
		gizmo->setTarget(currentSelected);
}

void Editor::renderGizmo(const Camera& camera)//(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{
	int width = Application::get().getWindowWidth(), height = Application::get().getWindowHeight();
	glm::mat4 projectionMatrix = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.f);
	glm::mat4 viewMatrix = glm::lookAt(camera.eye, camera.o, camera.up);

	gizmo->render(projectionMatrix, viewMatrix);
}

void Editor::renderUI()
{
	if (currentSelected != nullptr)
		currentSelected->drawUI();
}