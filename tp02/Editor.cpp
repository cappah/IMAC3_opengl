#include "Editor.h"

Editor::Editor(MaterialUnlit* _unlitMaterial) : m_currentSelected(nullptr)
{
	m_gizmo = new Gizmo(_unlitMaterial, this);
}

void Editor::changeCurrentSelected(Entity* entity)
{
	m_currentSelected = entity;
	
	if(m_gizmo != nullptr && entity != nullptr)
		m_gizmo->setTarget(m_currentSelected);
}

void Editor::renderGizmo(const Camera& camera)//(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{
	int width = Application::get().getWindowWidth(), height = Application::get().getWindowHeight();
	glm::mat4 projectionMatrix = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.f);
	glm::mat4 viewMatrix = glm::lookAt(camera.eye, camera.o, camera.up);

	m_gizmo->render(projectionMatrix, viewMatrix);
}

void Editor::renderUI()
{
	if (m_currentSelected != nullptr)
		m_currentSelected->drawUI();
}

bool Editor::testGizmoIntersection(const Ray & ray)
{

	m_gizmoTranslationDirection = m_gizmo->checkIntersection(ray, m_gizmoTranslationAnchor);
	if (m_gizmoTranslationDirection == Gizmo::GIZMO_ARROW_NONE)
		return false;
	else
		return true;
}

void Editor::beginMoveGizmo()
{
	m_isMovingGizmo = true;
}

bool Editor::isMovingGizmo()
{
	return m_isMovingGizmo;
}

void Editor::endMoveGizmo()
{
	m_isMovingGizmo = false;
}

void Editor::moveGizmo(const Ray & ray)
{
	float t;
	glm::vec3 normal;
	glm::vec3 normalX(1, 0, 0);
	glm::vec3 normalY(0, 1, 0);
	glm::vec3 normalZ(0, 0, 1);

	glm::vec3 direction;
	glm::vec3 directionX(1, 0, 0);
	glm::vec3 directionY(0, 1, 0);
	glm::vec3 directionZ(0, 0, 1);

	glm::vec3 tmpOldTranslationAnchor(m_gizmoTranslationAnchor);

	if (m_gizmoTranslationDirection == Gizmo::GIZMO_ARROW_X)
	{
		direction = directionX;

		if (glm::dot(normalY, ray.getDirection()) < 0.0001f)
			normal = normalZ;
		else
			normal = normalY;
			
		if (ray.intersectPlane(m_gizmoTranslationAnchor, normal, &t))
		{
			m_gizmoTranslationAnchor = ray.at(t);

			glm::vec3 deltaTranslation = m_gizmoTranslationAnchor - tmpOldTranslationAnchor;

			deltaTranslation = dot(deltaTranslation, direction) * direction;

			m_gizmo->translate(deltaTranslation);
		}
	}
	else if (m_gizmoTranslationDirection == Gizmo::GIZMO_ARROW_Y)
	{
		direction = directionY;

		if (glm::dot(normalZ, ray.getDirection()) < 0.0001f)
			normal = normalX;
		else
			normal = normalZ;

		if (ray.intersectPlane(m_gizmoTranslationAnchor, normal, &t))
		{
			m_gizmoTranslationAnchor = ray.at(t);

			glm::vec3 deltaTranslation = m_gizmoTranslationAnchor - tmpOldTranslationAnchor;

			deltaTranslation = dot(deltaTranslation, direction) * direction;

			m_gizmo->translate(deltaTranslation);
		}
	}
	else
	{
		direction = directionZ;

		if (glm::dot(normalX, ray.getDirection()) < 0.0001f)
			normal = normalY;
		else
			normal = normalX;

		if (ray.intersectPlane(m_gizmoTranslationAnchor, normal, &t))
		{
			m_gizmoTranslationAnchor = ray.at(t);

			glm::vec3 deltaTranslation = m_gizmoTranslationAnchor - tmpOldTranslationAnchor;

			deltaTranslation = dot(deltaTranslation, direction) * direction;

			m_gizmo->translate(deltaTranslation);
		}
	}
}
