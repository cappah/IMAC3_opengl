#pragma once

#include "Path.h"

class PathManager
{


public:
	PathManager();
	~PathManager();

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

