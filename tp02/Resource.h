#pragma once

#include "FileHandler.h"

class Resource
{
private:
	FileHandler::CompletePath m_path;

public:
	virtual void init(const FileHandler::CompletePath& path)
	{
		m_path = path;
	}
};