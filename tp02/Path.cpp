#include "Path.h"

Path::Path()
{
}

Path::~Path()
{
}

std::vector<PathPoint>::iterator Path::begin()
{
	return m_pathPoints.begin();
}

std::vector<PathPoint>::iterator Path::end()
{
	return m_pathPoints.end();
}

int Path::getPathId() const
{
	return m_pathId;
}

void Path::setPathId(int id)
{
	m_pathId = id;
}
