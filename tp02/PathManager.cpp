#include "PathManager.h"

PathManager::~PathManager()
{
	delete m_material;
}

void PathManager::updatePathId(int pathId, int oldPathId, PathPoint* pathPoint)
{
	auto oldPath = findPath(oldPathId);
	auto newPath = findPath(pathId);

	if (oldPath != nullptr)
	{
		if (newPath == nullptr)
			m_paths.push_back(new Path(pathId));

		oldPath->erase(pathPoint);
		newPath->push_back(pathPoint);

		newPath->sort();

		if (oldPath->size() == 0)
		{
			m_paths.erase(std::find(m_paths.begin(), m_paths.end(), oldPath));
			delete oldPath;
		}
	}
}

void PathManager::updatePointIdx(int pathId)
{
	auto path = findPath(pathId);

	path->sort();
}

Path* PathManager::findPath(int pathId)
{
	auto pathIt = std::find_if(m_paths.begin(), m_paths.end(), [pathId](const Path& p) { return p.getPathId() == pathId; });

	if (pathIt != m_paths.end())
		return *pathIt;
	else
		return nullptr;
}

PathPoint* PathManager::findPointInPath(int pathId, int pointIdx)
{
	auto path = findPath(pathId);

	if (path == nullptr)
		return nullptr;
	else
	{
		auto pathPointIt = std::find_if(path->begin(), path->end(), [pointIdx](const PathPoint& p) { return p.getPointIdx() == pointIdx; });

		if (pathPointIt != path->end())
			return *pathPointIt;
		else
			return nullptr;
	}
}

void PathManager::add(PathPoint * pathPoint)
{
	auto path = findPath(pathPoint->getPathId());
	if (path != nullptr)
	{
		path->insert(pathPoint->getPointIdx(), pathPoint);
	}
	else
	{
		m_paths.push_back(new Path(pathPoint->getPathId()));
		m_paths.back()->push_back(pathPoint);
	}
}

void PathManager::erase(PathPoint * pathPoint)
{
	auto path = findPath(pathPoint->getPathId());
	if (path != nullptr)
	{
		path->erase(pathPoint);
		delete pathPoint;

		if (path->size() == 0)
		{
			m_paths.erase(std::find(m_paths.begin(), m_paths.end(), path));
			delete path;
		}
	}
}

void PathManager::render(const glm::mat4 & projection, const glm::mat4 & view)
{
	m_material->use();
	for (auto& path : m_paths)
	{
		glm::mat4 MVP = projection * view;
		glm::mat4 normalMatrix = glm::mat4(1);

		m_material->setUniform_MVP(MVP);
		m_material->setUniform_normalMatrix(normalMatrix);

		path->draw();
	}
}
