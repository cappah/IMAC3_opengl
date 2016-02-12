#pragma once

#include <vector>
#include "PathPoint.h"
#include "BSpline.h"

class Path
{
private:
	int m_pathId;
	std::vector<PathPoint> m_pathPoints;

public:
	Path();
	~Path();

	std::vector<PathPoint>::iterator begin();
	std::vector<PathPoint>::iterator end();


	int getPathId() const;
	void setPathId(int id);
};

