#pragma once

#include <vector>
#include "PathPoint.h"
#include "BSpline.h"

class Path
{
private:
	Math::BSpline m_spline;
	std::vector<PathPoint> m_pathPoints;

public:
	Path();
	~Path();
};

