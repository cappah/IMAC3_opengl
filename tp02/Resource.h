#pragma once

#include "FileHandler.h"
#include "IDrawableInInspector.h"

class Scene;

class Resource : public IDrawableInInspector
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

	virtual void drawInInspector(Scene & scene, const std::vector<IDrawableInInspector*>& selection) override;
	virtual void drawInInspector(Scene & scene) override;
};