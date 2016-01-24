#include "Editor.h"
#include "Scene.h"

Editor::Editor(MaterialUnlit* _unlitMaterial) : m_isGizmoVisible(true), m_isMovingGizmo(false), m_isUIVisible(true)
{
	m_gizmo = new Gizmo(_unlitMaterial, this);
}

void Editor::changeCurrentSelected(Entity* entity)
{
	m_gizmo->setTarget(nullptr);
	m_currentSelected.clear();
	addCurrentSelected(entity);
}

void Editor::changeCurrentSelected(std::vector<Entity*> entities)
{
	m_gizmo->setTarget(nullptr);
	m_currentSelected.clear();
	for (auto& e : entities)
	{
		addCurrentSelected(e);
	}
}

void Editor::addCurrentSelected(Entity * entity)
{
	if (entity == nullptr)
		return;

	m_currentSelected.push_back(entity);

	if (m_gizmo != nullptr)
	{
		if(m_currentSelected.size() == 1)
			m_gizmo->setTarget(m_currentSelected.front()); // set a unique target
		else
			m_gizmo->setTargets(m_currentSelected); //set multiple targets
	}
}

void Editor::removeCurrentSelected(Entity * entity)
{
	if (entity == nullptr)
		return;

	auto findIt = std::find(m_currentSelected.begin(), m_currentSelected.end(), entity);

	if (findIt != m_currentSelected.end())
	{
		m_currentSelected.erase(findIt);
	}

	if (m_gizmo != nullptr)
	{
		if (m_currentSelected.size() == 1)
			m_gizmo->setTarget(m_currentSelected.front()); // set a unique target
		else
			m_gizmo->setTargets(m_currentSelected); //set multiple targets
	}
}

void Editor::toggleCurrentSelected(Entity* entity)
{
	if (entity == nullptr)
		return;

	auto findIt = std::find(m_currentSelected.begin(), m_currentSelected.end(), entity);

	if (findIt != m_currentSelected.end())
		removeCurrentSelected(entity);
	else
		addCurrentSelected(entity);
}

void Editor::renderGizmo(const Camera& camera)//(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{
	if (!m_isGizmoVisible)
		return;

	int width = Application::get().getWindowWidth(), height = Application::get().getWindowHeight();
	glm::mat4 projectionMatrix = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.f);
	glm::mat4 viewMatrix = glm::lookAt(camera.eye, camera.o, camera.up);

	m_gizmo->render(projectionMatrix, viewMatrix);
}

void Editor::renderUI(Scene& scene)
{
	if (!m_isUIVisible)
		return;

	if (ImGui::RadioButton("colliders visibility", scene.getAreCollidersVisible()))
	{
		scene.toggleColliderVisibility();
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("debug deferred visibility", scene.getIsDebugDeferredVisible()))
	{
		scene.toggleDebugDeferredVisibility();
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("gizmo visibility", m_isGizmoVisible))
	{
		toggleGizmoVisibility();
	}


	int entityId = 0;

	if (ImGui::Button("add empty entity"))
	{
		auto newEntity = new Entity(&scene);
		auto colliderRenderer = new MeshRenderer(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get("wireframe"));
		auto newCollider = new BoxCollider(colliderRenderer);
		newEntity->add(newCollider);
	}
	ImGui::SameLine();
	if (ImGui::Button("add pointLight"))
	{
		auto newEntity = new Entity(&scene);
		auto colliderRenderer = new MeshRenderer(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get("wireframe"));
		auto newCollider = new BoxCollider(colliderRenderer);
		auto light = new PointLight();
		newEntity->add(newCollider).add(light);
	}
	ImGui::SameLine();
	if (ImGui::Button("add directionalLight"))
	{
		auto newEntity = new Entity(&scene);
		auto colliderRenderer = new MeshRenderer(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get("wireframe"));
		auto newCollider = new BoxCollider(colliderRenderer);
		auto light = new DirectionalLight();
		newEntity->add(newCollider).add(light);
	}
	ImGui::SameLine();
	if (ImGui::Button("add spotLight"))
	{
		auto newEntity = new Entity(&scene);
		auto colliderRenderer = new MeshRenderer(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get("wireframe"));
		auto newCollider = new BoxCollider(colliderRenderer);
		auto light = new SpotLight();
		newEntity->add(newCollider).add(light);
	}

	if (ImGui::Button("add cube"))
	{
		auto newEntity = new Entity(&scene);
		auto colliderRenderer = new MeshRenderer(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get("wireframe"));
		auto newCollider = new BoxCollider(colliderRenderer);
		auto meshRenderer = new MeshRenderer(MeshFactory::get().get("cube"), MaterialFactory::get().get("brick"));
		newEntity->add(newCollider).add(meshRenderer);
	}

	if (!m_currentSelected.empty())
	{
		ImGui::Begin("selected entities");
			for(auto selected : m_currentSelected)
			{
				ImGui::PushID(entityId);
				if(ImGui::CollapsingHeader( ("entity "+patch::to_string(entityId)).c_str() ))
					selected->drawUI();
				ImGui::PopID();

				entityId++;
			}
		ImGui::End();
	}
		
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

Entity* Editor::duplicateSelected()
{
	if (m_currentSelected.empty())
		return nullptr;

	std::vector<Entity*> newEntities;

	for(auto selected : m_currentSelected)
		newEntities.push_back( new Entity(*selected) ); //copy the entity

	changeCurrentSelected(newEntities); //change selection, to select the copy

	return newEntities.front();
}

void Editor::deleteSelected(Scene& scene)
{
	if (m_currentSelected.empty())
		return;

	for (int i = 0; i < m_currentSelected.size(); i++)
		scene.erase( m_currentSelected[i] );

	changeCurrentSelected(nullptr);

}

void Editor::toggleUIVisibility()
{
	m_isUIVisible = !m_isUIVisible;
}

void Editor::toggleGizmoVisibility()
{
	m_isGizmoVisible = !m_isGizmoVisible;
}

void Editor::toggleDebugVisibility(Scene& scene)
{
	toggleUIVisibility();

	scene.setAreCollidersVisible(m_isUIVisible);
	scene.setIsDebugDeferredVisible(m_isUIVisible);
	m_isGizmoVisible = m_isUIVisible;
}
