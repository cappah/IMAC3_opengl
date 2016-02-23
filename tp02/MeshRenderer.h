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
	std::string meshName;
	Mesh* mesh;

	std::string materialName;
	Material3DObject* material;

public:
	MeshRenderer(Mesh* _mesh = nullptr, Material3DObject* _material = nullptr);
	virtual ~MeshRenderer();

	virtual void drawUI(Scene& scene) override;
	virtual void eraseFromScene(Scene& scene) override;
	virtual Component* clone(Entity* entity) override;
	virtual void addToScene(Scene& scene) override;

	void setMesh(Mesh* _mesh);
	void setMaterial(Material3DObject* _material);

	Material3DObject* getMaterial() const;
	Mesh* getMesh() const;

	std::string getMaterialName() const;
	std::string getMeshName() const;

	glm::vec3 getOrigin() const;
};

