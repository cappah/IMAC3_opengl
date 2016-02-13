#include "Path.h"

Path::Path(int pathId = 0): m_pathId(pathId)
{
}

Path::~Path()
{
}

std::vector<PathPoint*>::iterator Path::begin()
{
	return std::vector<PathPoint*>::iterator();
}

std::vector<PathPoint*>::iterator Path::end()
{
	return std::vector<PathPoint*>::iterator();
}

void Path::push_back(PathPoint * p)
{
	m_pathPoints.push_back(p);
}

void Path::insert(int idx, PathPoint * p)
{
	auto findIt = std::find_if(m_pathPoints.begin(), m_pathPoints.end(), [idx](const PathPoint* p) { return p->getPointIdx() > idx; });

	if (findIt == m_pathPoints.begin())
		m_pathPoints.insert(m_pathPoints.begin(), p);
	else if (findIt == m_pathPoints.end())
		m_pathPoints.push_back(p);
	else
		m_pathPoints.insert(findIt-1, p);
}

void Path::pop_back()
{
	m_pathPoints.pop_back();
}

void Path::erase(int idx)
{
	m_pathPoints.erase(m_pathPoints.begin() + idx);
}

void Path::erase(PathPoint* p)
{
	auto findIt = std::find(m_pathPoints.begin(), m_pathPoints.end(), p);
	if(findIt != m_pathPoints.end())
		m_pathPoints.erase(findIt);
}

void Path::clear()
{
	m_pathPoints.clear();
}

void Path::append(const std::vector<PathPoint*>& points)
{
	for (auto& p : points)
	{
		m_pathPoints.push_back(p);
	}
}

int Path::size()
{
	return m_pathPoints.size();
}

void Path::sort()
{
	std::sort(m_pathPoints.begin(), m_pathPoints.end(), [](const PathPoint* a, const PathPoint* b) { return a->getPointIdx() < b->getPointIdx(); });
}

PathPoint * Path::getPathPoint(int idx) const
{
	assert(idx >= 0 && idx < m_pathPoints.size());

	return m_pathPoints[idx];
}

glm::vec3 Path::get(float t) const
{
	int idx = (int)(t * m_pathPoints.size() - 0.999f);

	assert(idx >= 0 && idx + 2 < m_pathPoints.size());

	glm::vec3 P0 = m_pathPoints[idx]->getPosition();
	glm::vec3 P1 = m_pathPoints[idx + 1]->getPosition();
	glm::vec3 P2 = m_pathPoints[idx + 2]->getPosition();

	return Math::getBSplinePoint<glm::vec3>(P0, P1, P2, t);
}


int Path::getPathId() const
{
	return m_pathId;
}

void Path::setPathId(int id)
{
	m_pathId = id;
}

void Path::updateVisual()
{
	//TODO
}

void Path::draw()
{
	//TODO
}
