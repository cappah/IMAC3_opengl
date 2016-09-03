#pragma once

#include "FileHandler.h"

class Resource
{
private:
	FileHandler::CompletePath m_completePath;

public:
	Resource()
	{}

	Resource(const FileHandler::CompletePath& completePath)
		:m_completePath(completePath)
	{}

	virtual void init(const FileHandler::CompletePath& completePath)
	{
		m_completePath = completePath;
	}

	const FileHandler::CompletePath& getCompletePath() const
	{
		return m_completePath;
	}
};