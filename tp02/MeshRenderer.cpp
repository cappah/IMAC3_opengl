#include "MeshRenderer.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

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
		material->drawUI();

		ImGui::InputFloat("specular power", &material->specularPower);

		ImGui::InputFloat2("texture repetition", &material->textureRepetition[0]);

		char tmpTxt[30];
		diffuseTextureName.copy(tmpTxt, glm::min(30, (int)diffuseTextureName.size()), 0);
		tmpTxt[diffuseTextureName.size()] = '\0';

		if (ImGui::InputText("diffuse texture name", tmpTxt, 20))
		{
			diffuseTextureName = tmpTxt;

			if (TextureFactory::get().contains(diffuseTextureName))
			{

				material->textureDiffuse = TextureFactory::get().get(diffuseTextureName);
			}
		}

		specularTextureName.copy(tmpTxt, glm::min(30, (int)specularTextureName.size()), 0);
		tmpTxt[specularTextureName.size()] = '\0';
		if (ImGui::InputText("specular texture name", tmpTxt, 20))
		{
			specularTextureName = tmpTxt;

			if (TextureFactory::get().contains(specularTextureName))
				material->textureSpecular = TextureFactory::get().get(specularTextureName);
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
