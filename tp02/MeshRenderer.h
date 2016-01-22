#pragma once

#include "Mesh.h"
#include "Materials.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

struct MeshRenderer
{
	Mesh* mesh;
	Material* material;

	MeshRenderer(Mesh* _mesh = nullptr, Material* _material = nullptr);

	void drawUI();
};

