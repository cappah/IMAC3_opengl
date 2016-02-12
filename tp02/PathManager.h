#pragma once

#include <algorithm>

#include "Path.h"

class PathManager
{
private:
	std::vector<Path> m_paths;

public:
	PathManager();
	~PathManager();

	void updatePathId(int pathId, int oldPathId);
	void updatePointIdx(int pointIdx, int oldPointOdx);

	Path* findPath(int pathId);
	PathPoint* findPointInPath(int pathId, int pointIdx);


// singleton implementation :
private:
	PathManager() {

	}

public:
	inline static PathManager& get()
	{
		static PathManager instance;

		return instance;
	}


	PathManager(const PathManager& other) = delete;
	void operator=(const PathManager& other) = delete;
};

