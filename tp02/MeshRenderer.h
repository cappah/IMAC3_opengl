#pragma once

#include "Mesh.h"
#include "Materials.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Component.h"
#include "ResourcePointer.h"
#include "IRenderableComponent.h"

//forward
class Entity;

class MeshRenderer : public Component, public IRenderableComponent
{
private:
	//%NOCOMMIT%
	//Mesh* mesh;
	//std::vector<Material3DObject*> material;
	ResourcePtr<Mesh> m_mesh;
	std::vector<ResourcePtr<Material>> m_materials;

public:
	MeshRenderer();
	MeshRenderer(ResourcePtr<Mesh> mesh, ResourcePtr<Material> material);
	virtual ~MeshRenderer();

	virtual void drawInInspector(Scene& scene) override;
	virtual void drawInInspector(Scene& scene, const std::vector<Component*>& components) override;

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
	virtual void load(const Json::Value& rootComponent) override;

	virtual const IDrawable& getDrawable(int drawableIndex) const override;
	virtual const Material& getDrawableMaterial(int drawableIndex) const override;
	virtual const int getDrawableCount() const override;
};

