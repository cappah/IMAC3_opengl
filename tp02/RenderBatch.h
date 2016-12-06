#pragma once

#include <glew/glew.h>
#include <vector>
#include <map>

#include "IDrawable.h"
#include "Materials.h"

class IRenderBatch
{
public:
	virtual void add(const IDrawable* drawable, const Material* material) = 0;
	virtual void render() const = 0;

	virtual void pushGlobalsToGPU() const = 0;
	virtual void pushExternalsToPGU() const = 0;
};

template<typename DrawableType, typename MaterialType>
class RenderBatch
{
private:
	GLuint m_programId;
	std::map<MaterialType*, std::vector<DrawableType*>> m_container;

public:
	virtual void add(const IDrawable* drawable, const Material* material) override;
	virtual void render() const override;
};

template<typename DrawableType, typename MaterialType>
inline void RenderBatch<DrawableType, MaterialType>::add(const IDrawable * drawable, const Material * material)
{
	assert(drawable != nullptr && material != nullptr);
	container[static_cast<MaterialType*>(material)]->push_back(static_cast<DrawableType*>(drawable));
}

template<typename DrawableType, typename MaterialType>
inline void RenderBatch<DrawableType, MaterialType>::render() const
{
	MaterialType* material = m_container.begin().first;

	// Use material
	material->use();

	// Push globals to GPU
	pushGlobalsToGPU();

	// For each material instance...
	for (auto& item : m_container)
	{
		MaterialType* materialInstance = item.first;

		// Push internals to GPU
		materialInstance->pushInternalsToGPU();

		// For each drawable...
		for (auto& drawable : item.second)
		{
			// Push externals to GPU
			pushExternalsToPGU();

			// Draw the drawable
			drawable->draw();
		}
	}
}
