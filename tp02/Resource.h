#pragma once

#include "FileHandler.h"
#include "IDrawableInInspector.h"

class Scene;
class ResourceFactoryBase;

class Resource : public IDrawableInInspector
{
	friend ResourceFactoryBase;

protected:
	FileHandler::CompletePath m_completePath;
	std::string m_name;

public:
	Resource();
	virtual ~Resource();
	Resource(const FileHandler::CompletePath& completePath);
	virtual void init(const FileHandler::CompletePath& completePath);
	const FileHandler::CompletePath& getCompletePath() const;
	const std::string& getName() const;

	virtual void drawInInspector(Scene & scene, const std::vector<IDrawableInInspector*>& selection) override;
	virtual void drawInInspector(Scene & scene) override;

	virtual void drawIconeInResourceTree();
	virtual void drawUIOnHovered();
	virtual void drawIconeInResourceField();

private:
	void setName(const std::string& name);
};