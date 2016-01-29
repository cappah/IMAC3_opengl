#include "MeshRenderer.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

MeshRenderer::MeshRenderer(Mesh* _mesh, Material* _material) : Component(MESH_RENDERER), mesh(_mesh), material(_material), meshName(""), materialName("")
{
	if (_mesh != nullptr)
		meshName = _mesh->name;

	if (_material != nullptr)
		materialName = _mesh->name;
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

void MeshRenderer::setMesh(Mesh * _mesh)
{
	if(_mesh != nullptr)
		meshName = _mesh->name;

	mesh = _mesh;
}

void MeshRenderer::setMaterial(Material * _material)
{
	if (_material != nullptr)
		materialName = _material->name;

	material = _material;
}

Material * MeshRenderer::getMaterial() const
{
	return material;
}

Mesh * MeshRenderer::getMesh() const
{
	return mesh;
}
