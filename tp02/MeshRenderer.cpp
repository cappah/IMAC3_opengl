#include "MeshRenderer.h"
#include "Scene.h"
#include "Entity.h"

MeshRenderer::MeshRenderer(Mesh* _mesh, Material* _material) : Component(MESH_RENDERER), mesh(_mesh), material(_material)
{

}

MeshRenderer::~MeshRenderer()
{
	mesh = nullptr;
	material = nullptr;
}

void MeshRenderer::drawUI()
{
	if (ImGui::CollapsingHeader("mesh renderer"))
	{
		//TODO
	}
}

void MeshRenderer::eraseFromScene(Scene & scene)
{
	scene.erase(this);
}

Component* MeshRenderer::clone(Entity* entity)
{
	MeshRenderer* newMeshRenderer = new MeshRenderer(*this);

	newMeshRenderer->attachToEntity(entity);

	return newMeshRenderer;
}

void MeshRenderer::addToScene(Scene& scene)
{
	scene.add(this);
}
