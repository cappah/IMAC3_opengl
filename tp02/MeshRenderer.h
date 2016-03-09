#pragma once

#include "Mesh.h"
#include "Materials.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Component.h"

//forward
class Entity;

class MeshRenderer : public Component
{
private:
	Mesh* mesh;
	std::vector<Material3DObject*> material;

	//for UI : 
	std::string meshName;
	std::string materialName;

public:
	MeshRenderer();
	MeshRenderer(Mesh* _mesh, Material3DObject* _material);
	virtual ~MeshRenderer();

	virtual void drawUI(Scene& scene) override;
	virtual void eraseFromScene(Scene& scene) override;
	virtual Component* clone(Entity* entity) override;
	virtual void addToScene(Scene& scene) override;
	virtual void addToEntity(Entity& entity) override;
	virtual void eraseFromEntity(Entity& entity) override;

	void setMesh(Mesh* _mesh);
	void addMaterial(Material3DObject* _material);
	void removeMaterial(int idx);
	void setMaterial(Material3DObject* _material, int idx);

	Material3DObject* getMaterial(int idx) const;
	Mesh* getMesh() const;

	std::string getMaterialName(int idx) const;
	std::string getMeshName() const;

	glm::vec3 getOrigin() const;

	void render(const glm::mat4& projection, const glm::mat4& view);

	virtual void save(Json::Value& rootComponent) const override;
	virtual void load(Json::Value& rootComponent) override;
};

