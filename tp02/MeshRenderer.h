#pragma once

#include "Mesh.h"
#include "Materials.h"

struct MeshRenderer
{
	Mesh* mesh;
	Material* material;

	MeshRenderer(Mesh* _mesh = nullptr, Material* _material = nullptr);
};

