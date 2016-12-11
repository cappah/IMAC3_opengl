#include "Resource.h"
#include "Project.h"
#include "Scene.h"

Resource::Resource()
{}

Resource::~Resource()
{}

Resource::Resource(const FileHandler::CompletePath& completePath)
	: m_completePath(completePath)
	, m_name(m_completePath.getFilename())
{}

void Resource::init(const FileHandler::CompletePath& completePath)
{
	m_completePath = completePath;
	m_name = m_completePath.getFilename();
	assert(!Project::isPathPointingInsideProjectFolder(m_completePath)); //path shouldn't be absolute
}

const FileHandler::CompletePath& Resource::getCompletePath() const
{
	return m_completePath;
}

const std::string& Resource::getName() const
{
	return m_name;
}

void Resource::setName(const std::string & name)
{
	m_name = name;
}

void Resource::drawInInspector(Scene & scene, const std::vector<IDrawableInInspector*>& selection)
{
	assert(false && "multiple edition of resource isn't implemented.");
}

void Resource::drawInInspector(Scene & scene)
{
	ImGui::Text(getName().c_str());
}

void Resource::drawIconeInResourceTree()
{
	ImGui::Dummy(ImVec2(1, 1));
}

void Resource::drawUIOnHovered()
{
	// Nothing
}

void Resource::drawIconeInResourceField()
{
	ImGui::Dummy(ImVec2(1, 1));
}
