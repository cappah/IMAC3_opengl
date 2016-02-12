#include "PathPoint.h"
//forwards : 
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"
#include "PathManager.h"

PathPoint::PathPoint(): Component(ComponentType::PATH_POINT)
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

void PathPoint::drawUI()
{
	if (ImGui::CollapsingHeader("Path point"))
	{
		int oldPathId = m_pathId;
		if (ImGui::InputInt("path id", &m_pathId))
		{
			PathManager::get().updatePathId(m_pathId, oldPathId);
		}
		int oldPointIdx = m_pointIdx;
		if (ImGui::InputInt("point index", &m_pointIdx))
		{
			PathManager::get().updatePointIdx(m_pathId, oldPathId);
		}
	}
}

void PathPoint::eraseFromScene(Scene & scene)
{
	scene.erase(this);
}

void PathPoint::addToScene(Scene & scene)
{
	scene.add(this);
}

Component* PathPoint::clone(Entity * entity)
{
	PathPoint* pathPoint = new PathPoint(*this);

	pathPoint->attachToEntity(entity);

	return pathPoint;
}
