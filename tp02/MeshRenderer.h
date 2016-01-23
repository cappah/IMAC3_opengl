#pragma once

#include "Mesh.h"
#include "Materials.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Component.h"

struct MeshRenderer : public Component
{
	Mesh* mesh;
	Material* material;

	MeshRenderer(Mesh* _mesh = nullptr, Material* _material = nullptr);

	virtual void drawUI() override;
	virtual void eraseFromScene(Scene& scene) override;
};

