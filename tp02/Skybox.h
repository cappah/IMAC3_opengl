#pragma once

#include "Mesh.h"
#include "Materials.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "glm/common.hpp"

#include "Factories.h"

class Skybox
{
	Mesh mesh;

	std::string materialName;
	MaterialSkybox material;

public:

	Skybox();
	~Skybox();

	void drawUI();

	void render(const glm::mat4& projection, const glm::mat4& view );
};

