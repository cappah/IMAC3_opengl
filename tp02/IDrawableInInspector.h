#pragma once

#include <vector>

template <typename T>
class IDrawableInInspector
{
	virtual void drawInInspector(std::vector<T> selection) = 0;
};