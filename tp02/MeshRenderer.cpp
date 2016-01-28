#include "MeshRenderer.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

MeshRenderer::MeshRenderer(Mesh* _mesh, Material* _material) : Component(MESH_RENDERER), mesh(_mesh), material(_material), meshName(""), materialName("")
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
		char tmpMaterialName[20];
		materialName.copy(tmpMaterialName, materialName.size());
		tmpMaterialName[materialName.size()] = '\0';

		if (ImGui::InputText("materialName", tmpMaterialName, 20))
		{
			materialName = tmpMaterialName;

			if (MaterialFactory::get().contains(materialName))
			{
				material = MaterialFactory::get().get(materialName);
			}
		}

		material->drawUI();

		char tmpMeshName[20];
		meshName.copy(tmpMeshName, meshName.size());
		tmpMeshName[meshName.size()] = '\0';

		if (ImGui::InputText("meshName", tmpMeshName, 20))
		{
			meshName = tmpMeshName;

			if (MeshFactory::get().contains(meshName))
			{
				mesh = MeshFactory::get().get(meshName);
			}
		}
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
