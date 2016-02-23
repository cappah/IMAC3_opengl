#include "MeshRenderer.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

MeshRenderer::MeshRenderer(Mesh* _mesh, Material3DObject* _material) : Component(MESH_RENDERER), mesh(_mesh), material(_material), meshName(""), materialName("")
{
	if (_mesh != nullptr)
		meshName = _mesh->name;

	if (_material != nullptr)
		materialName = _material->name;
}

MeshRenderer::~MeshRenderer()
{
	mesh = nullptr;
	material = nullptr;
}

void MeshRenderer::drawUI(Scene& scene)
{
	if (ImGui::CollapsingHeader("mesh renderer"))
	{
		char tmpMaterialName[20];
		materialName.copy(tmpMaterialName, materialName.size());
		tmpMaterialName[materialName.size()] = '\0';

		if (ImGui::InputText("materialName", tmpMaterialName, 20))
		{
			materialName = tmpMaterialName;

			if (MaterialFactory::get().contains<Material3DObject>(materialName))
			{
				Material3DObject* tmpMat = MaterialFactory::get().get<Material3DObject>(materialName);
				if (tmpMat != nullptr)
					material = tmpMat;
			}
		}

		material->drawUI();

		char tmpMeshName[20];
		meshName.copy(tmpMeshName, meshName.size());
		tmpMeshName[meshName.size()] = '\0';

		if (ImGui::InputText("meshName", tmpMeshName, 20))
		{
			if (MeshFactory::get().contains(tmpMeshName))
			{
				setMesh(MeshFactory::get().get(tmpMeshName));
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

	if (m_entity != nullptr)
	{
		auto collider = static_cast<Collider*>(m_entity->getComponent(Component::COLLIDER));
		if (collider != nullptr)
			collider->coverMesh(*mesh);
	}
}

void MeshRenderer::setMaterial(Material3DObject * _material)
{
	if (_material != nullptr)
		materialName = _material->name;

	material = _material;
}

Material3DObject * MeshRenderer::getMaterial() const
{
	return material;
}

Mesh * MeshRenderer::getMesh() const
{
	return mesh;
}

std::string MeshRenderer::getMaterialName() const
{
	return materialName;
}

std::string MeshRenderer::getMeshName() const
{
	return meshName;
}

glm::vec3 MeshRenderer::getOrigin() const
{
	return mesh->origin;
}
