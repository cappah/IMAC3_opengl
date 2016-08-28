#pragma once

#include "Mesh.h"
#include "Materials.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Component.h"
#include "ResourcePointer.h"

//forward
class Entity;

class MeshRenderer : public Component
{
private:
	//%NOCOMMIT%
	//Mesh* mesh;
	//std::vector<Material3DObject*> material;
	ResourcePtr<Mesh> mesh;
	std::vector<ResourcePtr<Material>> material;

	//for UI : 
	std::string meshName;
	std::string materialName;

public:
	MeshRenderer();
	MeshRenderer(ResourcePtr<Mesh> _mesh, ResourcePtr<Material> _material);
	virtual ~MeshRenderer();

	virtual void drawUI(Scene& scene) override;
	virtual void eraseFromScene(Scene& scene) override;
	virtual Component* clone(Entity* entity) override;
	virtual void addToScene(Scene& scene) override;
	virtual void addToEntity(Entity& entity) override;
	virtual void eraseFromEntity(Entity& entity) override;

	void setMesh(ResourcePtr<Mesh> _mesh);
	void addMaterial(ResourcePtr<Material> _material);
	void removeMaterial(int idx);
	void setMaterial(ResourcePtr<Material> _material, int idx);

	const Material* getMaterial(int idx) const;
	const Mesh* getMesh() const;

	std::string getMaterialName(int idx) const;
	std::string getMeshName() const;

	glm::vec3 getOrigin() const;

	void render(const glm::mat4& projection, const glm::mat4& view);

	virtual void save(Json::Value& rootComponent) const override;
	virtual void load(Json::Value& rootComponent) override;
};

