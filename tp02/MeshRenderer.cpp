#include "MeshRenderer.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

MeshRenderer::MeshRenderer() : Component(MESH_RENDERER), mesh(MeshFactory::get().get("default")), meshName("default"), materialName("default")
{
	if(mesh != nullptr)
		meshName = mesh->name;
	materialName = '\0';

	material.push_back(MaterialFactory::get().get<Material3DObject>("default"));
}

MeshRenderer::MeshRenderer(Mesh* _mesh, Material3DObject* _material) : Component(MESH_RENDERER), mesh(_mesh), meshName("default"), materialName("default")
{
	if (mesh != nullptr)
		meshName = mesh->name;
	materialName = '\0';

	material.push_back(_material);
}

MeshRenderer::~MeshRenderer()
{
	mesh = nullptr;
	material.clear();
}

void MeshRenderer::drawUI(Scene& scene)
{
	//char tmpMaterialName[20];
	//materialName.copy(tmpMaterialName, materialName.size());
	//tmpMaterialName[materialName.size()] = '\0';

	//if (ImGui::InputText("materialName", tmpMaterialName, 20))
	//{
	//	materialName = tmpMaterialName;

	//	if (MaterialFactory::get().contains<Material3DObject>(materialName))
	//	{
	//		Material3DObject* tmpMat = MaterialFactory::get().get<Material3DObject>(materialName);
	//		if (tmpMat != nullptr)
	//			material = tmpMat;
	//	}
	//}

	char tmpMaterialName[20];
	materialName.copy(tmpMaterialName, materialName.size());
	tmpMaterialName[materialName.size()] = '\0';

	if(ImGui::InputText("materialName", tmpMaterialName, 20))
		materialName = tmpMaterialName;
	ImGui::SameLine();
	if (ImGui::Button("add"))
	{
		if (MaterialFactory::get().contains<Material3DObject>(materialName)) {
			Material3DObject* tmpMat = MaterialFactory::get().get<Material3DObject>(materialName);
			if (tmpMat != nullptr)
				material.push_back(tmpMat);
		}
	}

	for (int i = 0; i < material.size(); i++)
	{
		ImGui::PushID(i);
		ImGui::Text(material[i]->name.c_str());
		if (material.size() > 1){
			ImGui::SameLine();
			if (ImGui::Button("remove")){
				material.erase(material.begin() + i);
			}
		}
		ImGui::PopID();
	}

	//material->drawUI();

	char tmpMeshName[20];
	meshName.copy(tmpMeshName, meshName.size());
	tmpMeshName[meshName.size()] = '\0';

	if (ImGui::InputText("meshName", tmpMeshName, 20))
	{
		meshName = tmpMeshName;

		if (MeshFactory::get().contains(tmpMeshName))
		{
			setMesh(MeshFactory::get().get(tmpMeshName));
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

void MeshRenderer::addToEntity(Entity& entity)
{
	entity.add(this);
}

void MeshRenderer::eraseFromEntity(Entity& entity)
{
	entity.erase(this);
}

void MeshRenderer::setMesh(Mesh* _mesh)
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

void MeshRenderer::addMaterial(Material3DObject* _material)
{
	material.push_back(_material);
}

void MeshRenderer::removeMaterial(int idx)
{
	assert(idx >= 0 && idx < material.size());

	material.erase(material.begin() + idx);
}

void MeshRenderer::setMaterial(Material3DObject * _material, int idx)
{
	assert(idx >= 0 && idx < material.size());

	material[idx] = _material;
}

//void MeshRenderer::setMaterial(Material3DObject * _material)
//{
//	if (_material != nullptr)
//		materialName = _material->name;
//
//	material = _material;
//}

Material3DObject * MeshRenderer::getMaterial(int idx) const
{
	return material[idx];
}

Mesh * MeshRenderer::getMesh() const
{
	return mesh;
}

std::string MeshRenderer::getMaterialName(int idx) const
{
	assert(idx >= 0 && idx < material.size());

	return material[idx]->name;
}

std::string MeshRenderer::getMeshName() const
{
	return meshName;
}

glm::vec3 MeshRenderer::getOrigin() const
{
	return mesh->origin;
}

void MeshRenderer::render(const glm::mat4 & projection, const glm::mat4 & view)
{
	glm::mat4 modelMatrix = entity()->getModelMatrix(); //get modelMatrix
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
	glm::mat4 mvp = projection * view * modelMatrix;

	int minMatMeshCount = std::min((int)material.size(), mesh->subMeshCount);
	for (int i = 0; i < minMatMeshCount; i++)
	{
		material[i]->use();
		material[i]->setUniform_MVP(mvp);
		material[i]->setUniform_normalMatrix(normalMatrix);
		if (mesh->getIsSkeletalMesh()) {
			for (int boneIdx = 0; boneIdx < mesh->getSkeleton()->getBoneCount(); boneIdx++)
				material[i]->setUniformBonesTransform(boneIdx, mesh->getSkeleton()->getBoneTransform(boneIdx));
		}
		material[i]->setUniformUseSkeleton(mesh->getIsSkeletalMesh());

		mesh->draw(i);	
	}
	//if there are more sub mesh than materials draw them with the last material
	for (int i = minMatMeshCount; i < mesh->subMeshCount; i++)
	{
		material.back()->use();
		material.back()->setUniform_MVP(mvp);
		material.back()->setUniform_normalMatrix(normalMatrix);
		if (mesh->getIsSkeletalMesh())
			for (int boneIdx = 0; boneIdx < mesh->getSkeleton()->getBoneCount(); boneIdx++)
				material[i]->setUniformBonesTransform(boneIdx, mesh->getSkeleton()->getBoneTransform(boneIdx));
		material[i]->setUniformUseSkeleton(mesh->getIsSkeletalMesh());

		mesh->draw(i);
	}
}

void MeshRenderer::save(Json::Value & rootComponent) const
{
	Component::save(rootComponent);

	rootComponent["meshName"] = mesh->name;

	rootComponent["materialCount"] = material.size();
	for (int i = 0; i < material.size(); i++)
		rootComponent["materialName"][i] = material[i]->name;
}

void MeshRenderer::load(Json::Value & rootComponent)
{
	Component::load(rootComponent);

	meshName = rootComponent.get("meshName", "").asString();
	mesh = MeshFactory::get().get(meshName);

	int materialCount = rootComponent.get("materialCount", 0).asInt();
	material.clear();
	for (int i = 0; i < materialCount; i++)
	{
		materialName = rootComponent["materialName"][i].asString();
		material.push_back(MaterialFactory::get().get<Material3DObject>(materialName));
		material.back()->initGL();
	}
	materialName = '\0'; //rootComponent.get("materialName", "").asString();


}
