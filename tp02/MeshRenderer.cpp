#include "stdafx.h"

#include "MeshRenderer.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

#include "EditorGUI.h"

MeshRenderer::MeshRenderer() : Component(MESH_RENDERER), mesh(getMeshFactory().getDefault("default")), meshName("default"), materialName("default")
{
	materialName = '\0';

	material.push_back(getMaterialFactory().getDefault("default"));
}

MeshRenderer::MeshRenderer(ResourcePtr<Mesh> _mesh, ResourcePtr<Material> _material) : Component(MESH_RENDERER), mesh(_mesh), meshName("default"), materialName("default")
{
	materialName = '\0';

	material.push_back(_material);
}

MeshRenderer::~MeshRenderer()
{
	mesh.reset();
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

	//	if (getMaterialFactory().contains<Material3DObject>(materialName))
	//	{
	//		Material3DObject* tmpMat = getMaterialFactory().get<Material3DObject>(materialName);
	//		if (tmpMat != nullptr)
	//			material = tmpMat;
	//	}
	//}

	char tmpMaterialName[20];
	materialName.copy(tmpMaterialName, materialName.size());
	tmpMaterialName[materialName.size()] = '\0';

	//%NOCOMMIT%
	/*if(ImGui::InputText("materialName", tmpMaterialName, 20))
		materialName = tmpMaterialName;
	ImGui::SameLine();
	if (ImGui::Button("add"))
	{
		if (getMaterialFactory().contains<Material3DObject>(materialName)) {
			Material3DObject* tmpMat = getMaterialFactory().get<Material3DObject>(materialName);
			if (tmpMat != nullptr)
				material.push_back(tmpMat);
		}
	}*/
	ResourcePtr<Material> materialQuery;
	EditorGUI::ResourceField<Material>(materialQuery, "materialName", tmpMaterialName, 20);
	if (materialQuery.isValid())
	{
		material.push_back(materialQuery);
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
	
	// TODO
	//if (ImGui::InputText("meshName", tmpMeshName, 20))
	//{
	//	meshName = tmpMeshName;

	//	if (MeshFactory().contains(tmpMeshName))
	//	{
	//		setMesh(MeshFactory().get(tmpMeshName));
	//	}
	//}

	ResourcePtr<Mesh> meshQuery;
	EditorGUI::ResourceField<Mesh>(meshQuery, "meshName", tmpMeshName, 20);
	if (meshQuery.isValid())
	{
		setMesh(meshQuery);
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

void MeshRenderer::setMesh(ResourcePtr<Mesh> _mesh)
{
	mesh = _mesh;

	if (m_entity != nullptr)
	{
		auto collider = static_cast<Collider*>(m_entity->getComponent(Component::COLLIDER));
		if (collider != nullptr)
			collider->coverMesh(*mesh);
	}
}

void MeshRenderer::addMaterial(ResourcePtr<Material> _material)
{
	material.push_back(_material);
}

void MeshRenderer::removeMaterial(int idx)
{
	assert(idx >= 0 && idx < material.size());

	material.erase(material.begin() + idx);
}

void MeshRenderer::setMaterial(ResourcePtr<Material> _material, int idx)
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

const Material* MeshRenderer::getMaterial(int idx) const
{
	return material[idx].get();
}

const Mesh* MeshRenderer::getMesh() const
{
	return mesh.get();
}

std::string MeshRenderer::getMaterialName(int idx) const
{
	assert(idx >= 0 && idx < material.size());

	return material[idx]->getCompletePath().getFilename();
}

std::string MeshRenderer::getMeshName() const
{
	return mesh->getCompletePath().getFilename();
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
		Material3DObject* castedMaterial = static_cast<Material3DObject*>(material[i].get()); //TODO : a enlever lors de l'upgrade du pipeline graphique.

		castedMaterial->use();
		castedMaterial->setUniform_MVP(mvp);
		castedMaterial->setUniform_normalMatrix(normalMatrix);
		if (mesh->getIsSkeletalMesh()) {
			for (int boneIdx = 0; boneIdx < mesh->getSkeleton()->getBoneCount(); boneIdx++)
				castedMaterial->setUniformBonesTransform(boneIdx, mesh->getSkeleton()->getBoneTransform(boneIdx));
		}
		castedMaterial->setUniformUseSkeleton(mesh->getIsSkeletalMesh());

		mesh->draw(i);	
	}
	//if there are more sub mesh than materials draw them with the last material
	for (int i = minMatMeshCount; i < mesh->subMeshCount; i++)
	{
		Material3DObject* castedMaterial = static_cast<Material3DObject*>(material.back().get()); //TODO : a enlever lors de l'upgrade du pipeline graphique.

		castedMaterial->use();
		castedMaterial->setUniform_MVP(mvp);
		castedMaterial->setUniform_normalMatrix(normalMatrix);
		if (mesh->getIsSkeletalMesh())
			for (int boneIdx = 0; boneIdx < mesh->getSkeleton()->getBoneCount(); boneIdx++)
				castedMaterial->setUniformBonesTransform(boneIdx, mesh->getSkeleton()->getBoneTransform(boneIdx));
		castedMaterial->setUniformUseSkeleton(mesh->getIsSkeletalMesh());

		mesh->draw(i);
	}
}

void MeshRenderer::save(Json::Value & rootComponent) const
{
	Component::save(rootComponent);

	mesh.save(rootComponent["mesh"]);

	//rootComponent["meshKey"] = mesh->name;

	rootComponent["materialCount"] = material.size();
	for (int i = 0; i < material.size(); i++)
		material[i].save(rootComponent["material"][i]);
}

void MeshRenderer::load(Json::Value & rootComponent)
{
	Component::load(rootComponent);

	mesh.load(rootComponent["mesh"]);

	int materialCount = rootComponent.get("materialCount", 0).asInt();
	material.clear();
	for (int i = 0; i < materialCount; i++)
	{
		ResourcePtr<Material> newMaterial(rootComponent["material"][i]);
		material.push_back(newMaterial);
		material.back()->initGL();
	}
	materialName = '\0'; //rootComponent.get("materialName", "").asString();

}
