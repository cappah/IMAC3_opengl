#include "PathManager.h"

PathManager::~PathManager()
{

}

void PathManager::updatePathId(int pathId, int oldPathId, PathPoint* pathPoint)
{
	auto oldPath = findPath(oldPathId);
	auto newPath = findPath(pathId);

	if (oldPath != nullptr)
	{
		if (newPath == nullptr)
			m_paths.push_back(Path(pathId));

		oldPath->erase(pathPoint);
		newPath->push_back(pathPoint);

		newPath->sort();
	}
}

void PathManager::updatePointIdx(int pointIdx, int oldPointOdx)
{
}

Path* PathManager::findPath(int pathId)
{
	auto pathIt = std::find_if(m_paths.begin(), m_paths.end(), [pathId](const Path& p) { return p.getPathId() == pathId; });

	if (pathIt != m_paths.end())
		return &(*pathIt);
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
			return &(*pathPointIt);
		else
			return nullptr;
	}
}
