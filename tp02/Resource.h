#pragma once

#include "FileHandler.h"

class Resource
{
protected:
	FileHandler::CompletePath m_completePath;

public:
	Resource()
	{}

	virtual ~Resource()
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