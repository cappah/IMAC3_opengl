#include "stdafx.h"

#include "PathPoint.h"
//forwards : 
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"
#include "PathManager.h"

PathPoint::PathPoint(): Component(ComponentType::PATH_POINT), m_pathId(0), m_pointIdx(0)
{
	
}

PathPoint::~PathPoint()
{

}

glm::vec3 PathPoint::getPosition()
{
	if (m_entity != nullptr)
		return m_entity->getTranslation();
	else
		return glm::vec3(0, 0, 0);
}

int PathPoint::getPointIdx() const
{
	return m_pointIdx;
}

int PathPoint::getPathId() const
{
	return m_pathId;
}

void PathPoint::drawUI(Scene& scene)
{
	int oldPathId = m_pathId;
	if (ImGui::InputInt("path id", &m_pathId))
	{
		scene.getPathManager().updatePathId(m_pathId, oldPathId, this);
	}
	if (ImGui::InputInt("point index", &m_pointIdx))
	{
		scene.getPathManager().updatePointIdx(m_pathId);
	}
	if (ImGui::Button("update visual"))
	{
		scene.getPathManager().updateVisual(m_pathId);
	}
}

void PathPoint::eraseFromScene(Scene& scene)
{
	scene.erase(this);
}

void PathPoint::addToScene(Scene& scene)
{
	scene.add(this);
}

Component* PathPoint::clone(Entity* entity)
{
	PathPoint* pathPoint = new PathPoint(*this);

	pathPoint->attachToEntity(entity);

	return pathPoint;
}

void PathPoint::addToEntity(Entity& entity)
{
	entity.add(this);
}

void PathPoint::eraseFromEntity(Entity& entity)
{
	entity.erase(this);
}

void PathPoint::save(Json::Value & rootComponent) const
{
	Component::save(rootComponent);

	rootComponent["pathId"] = m_pathId;
	rootComponent["pointIdx"] = m_pointIdx;
}

void PathPoint::load(const Json::Value & rootComponent)
{
	Component::load(rootComponent);

	m_pathId = rootComponent.get("pathId", 0).asInt();
	m_pointIdx = rootComponent.get("pointIdx", 0).asInt();
}
