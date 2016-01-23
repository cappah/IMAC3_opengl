#include "MeshRenderer.h"
#include "Scene.h"

MeshRenderer::MeshRenderer(Mesh* _mesh, Material* _material) : Component(MESH_RENDERER), mesh(_mesh), material(_material)
{

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
