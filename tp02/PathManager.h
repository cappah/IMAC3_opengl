#pragma once

#include <algorithm>

#include "Path.h"
#include "Materials.h"

class PathManager
{
private:
	Material* m_material;
	std::vector<Path*> m_paths;

public:
	PathManager();
	~PathManager();

	void updatePathId(int pathId, int oldPathId, PathPoint* pathPoint);
	void updatePointIdx(int pathId);

	Path* findPath(int pathId);
	PathPoint* findPointInPath(int pathId, int pointIdx);

	void add(PathPoint* pathPoint);
	void erase(PathPoint* pathPoint);

	void render(const glm::mat4& projection, const glm::mat4& view);


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

