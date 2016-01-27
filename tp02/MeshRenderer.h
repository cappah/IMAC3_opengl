#pragma once

#include "Mesh.h"
#include "Materials.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Component.h"

//forward
class Entity;

struct MeshRenderer : public Component
{
	Mesh* mesh;

	std::string materialName;
	Material* material;

	std::string diffuseTextureName;
	std::string specularTextureName;

	MeshRenderer(Mesh* _mesh = nullptr, Material* _material = nullptr);
	virtual ~MeshRenderer();

	virtual void drawUI() override;
	virtual void eraseFromScene(Scene& scene) override;
	virtual Component* clone(Entity* entity) override;
	virtual void addToScene(Scene& scene) override;
};

