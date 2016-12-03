#pragma once

#include "FileHandler.h"

class Resource
{
protected:
	FileHandler::CompletePath m_completePath;

public:
	Resource();
	virtual ~Resource();
	Resource(const FileHandler::CompletePath& completePath);
	virtual void init(const FileHandler::CompletePath& completePath);
	const FileHandler::CompletePath& getCompletePath() const;
	const std::string& getName() const;
};