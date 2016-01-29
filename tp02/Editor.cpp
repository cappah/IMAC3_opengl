#include "Editor.h"
#include "Scene.h"
#include "Application.h" //forward
#include "Factories.h" //forward



///////////////////////////////// INSPECTOR

Inspector::Inspector()
{

}

Inspector::~Inspector()
{

}

void Inspector::drawUI(const std::vector<PointLight*>& pointLights)
{
	if (pointLights.size() == 0)
		return;

	if (ImGui::CollapsingHeader("point light"))
	{
		floatValue = pointLights[0]->intensity;
		if (ImGui::SliderFloat("light intensity", &floatValue, 0.f, 50.f))
		{
			for (auto& light : pointLights)
			{
				light->intensity = floatValue;
			}
		}
		vector3Value = pointLights[0]->color;
		if (ImGui::ColorEdit3("light color", &vector3Value[0]))
		{
			for (auto& light : pointLights)
			{
				light->color = vector3Value;
			}
		}
	}
}

void Inspector::drawUI(const std::vector<DirectionalLight*>& directionalLights)
{
	if (directionalLights.size() == 0)
		return;

	if (ImGui::CollapsingHeader("directional light"))
	{
		floatValue = directionalLights[0]->intensity;
		if (ImGui::SliderFloat("light intensity", &floatValue, 0.f, 10.f))
		{
			for (auto& light : directionalLights)
			{
				light->intensity = floatValue;
			}
		}
		vector3Value = directionalLights[0]->color;
		if (ImGui::ColorEdit3("light color", &vector3Value[0]))
		{
			for (auto& light : directionalLights)
			{
				light->color = vector3Value;
			}
		}
	}
}

void Inspector::drawUI(const std::vector<SpotLight*>& spotLights)
{
	if (spotLights.size() == 0)
		return;

	if (ImGui::CollapsingHeader("spot light"))
	{
		floatValue = spotLights[0]->intensity;
		if (ImGui::SliderFloat("light intensity", &floatValue, 0.f, 50.f))
		{
			for (auto& light : spotLights)
			{
				light->intensity = floatValue;
			}
		}
		vector3Value = spotLights[0]->color;
		if (ImGui::ColorEdit3("light color", &vector3Value[0]))
		{
			for (auto& light : spotLights)
			{
				light->color = vector3Value;
			}
		}
		floatValue = spotLights[0]->angle;
		if (ImGui::SliderFloat("light angles", &floatValue, 0.f, glm::pi<float>()))
		{
			for (auto& light : spotLights)
			{
				light->angle = floatValue;
			}
		}
	}
}

void Inspector::drawUI(const std::vector<Entity*>& entities)
{
	if (entities.size() == 0)
		return;

	std::string tmpName = entities[0]->getName();
	tmpName.copy(textValue, tmpName.size(), 0);
	textValue[tmpName.size()] = '\0';

	if (ImGui::InputText("name", textValue, 20))
	{
		for (auto& entity : entities)
		{
			entity->setName(textValue);
		}
	}

	vector3Value = entities[0]->getEulerRotation();
	if (ImGui::SliderFloat3("rotation", &vector3Value[0], 0, 2 * glm::pi<float>()))
	{
		for (auto& entity : entities)
		{
			entity->setEulerRotation(vector3Value);
			//entity->setRotation(glm::quat(vector3Value));
			entity->applyTransform();
		}
	}

	vector3Value = entities[0]->getScale();
	if (ImGui::InputFloat3("scale", &vector3Value[0]))
	{
		for (auto& entity : entities)
		{
			entity->setScale(vector3Value);
			entity->applyTransform();
		}
	}
}

void Inspector::drawUI(const std::vector<MeshRenderer*>& meshRenderers)
{
	if (meshRenderers.size() == 0)
		return;

	std::string tmpName = meshRenderers[0]->getMaterialName();
	tmpName.copy(textValue, tmpName.size(), 0);
	textValue[tmpName.size()] = '\0';

	if (ImGui::InputText("materialName", textValue, 20))
	{
		for (auto& meshRenderer : meshRenderers)
		{
			if (MaterialFactory::get().contains(textValue))
			{
				meshRenderer->setMaterial( MaterialFactory::get().get(textValue) );
			}
		}
	}

	meshRenderers[0]->getMaterial()->drawUI();

	tmpName = meshRenderers[0]->getMeshName();
	tmpName.copy(textValue, tmpName.size(), 0);
	textValue[tmpName.size()] = '\0';

	if (ImGui::InputText("meshName", textValue, 20))
	{
		for (auto& meshRenderer : meshRenderers)
		{
			if (MeshFactory::get().contains(textValue))
			{
				meshRenderer->setMesh( MeshFactory::get().get(textValue) );
			}
		}
	}
}

void Inspector::drawUI(const std::vector<Collider*>& colliders)
{
	if (colliders.size() == 0)
		return;

	vector3Value = colliders[0]->offsetPosition;
	if (ImGui::InputFloat3("offset position", &vector3Value[0]))
	{
		for (auto& collider : colliders)
		{
			collider->setOffsetPosition(vector3Value);
		}
	}
	vector3Value = colliders[0]->offsetScale;
	if (ImGui::InputFloat3("offset scale", &vector3Value[0]))
	{
		for (auto& collider : colliders)
		{
			collider->setOffsetScale(vector3Value);
		}
	}
}



/////////////////////////////////// EDITOR

Editor::Editor(MaterialUnlit* _unlitMaterial) : m_isGizmoVisible(true), m_isMovingGizmo(false), m_isUIVisible(true), m_multipleEditing(false)
{
	m_gizmo = new Gizmo(_unlitMaterial, this);
}

void Editor::changeCurrentSelected(Entity* entity)
{
	clearSelectedComponents();

	m_gizmo->setTarget(nullptr);
	m_currentSelected.clear();
	addCurrentSelected(entity);
}

void Editor::changeCurrentSelected(std::vector<Entity*> entities)
{
	clearSelectedComponents();

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

	refreshSelectedComponents(false);
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

	refreshSelectedComponents(true);
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

void Editor::clearSelectedComponents()
{
	m_pointLights.clear();
	m_directionlLights.clear();
	m_spotLights.clear();
	m_meshRenderers.clear();
	m_colliders.clear();
}

void Editor::refreshSelectedComponents(bool clearComponentLists)
{
	if (clearComponentLists)
	{
		m_pointLights.clear();
		m_directionlLights.clear();
		m_spotLights.clear();
		m_meshRenderers.clear();
		m_colliders.clear();
	}

	for (auto& selected : m_currentSelected)
	{
		PointLight* pointLight = static_cast<PointLight*>(selected->getComponent(Component::POINT_LIGHT));
		DirectionalLight* directionalLight = static_cast<DirectionalLight*>(selected->getComponent(Component::DIRECTIONAL_LIGHT));
		SpotLight* spotLight = static_cast<SpotLight*>(selected->getComponent(Component::SPOT_LIGHT));
		MeshRenderer* meshRenderer = static_cast<MeshRenderer*>(selected->getComponent(Component::MESH_RENDERER));
		Collider* collider = static_cast<Collider*>(selected->getComponent(Component::COLLIDER));

		if (pointLight != nullptr)
		{
			m_pointLights.push_back(pointLight);
		}
		if (directionalLight != nullptr)
		{
			m_directionlLights.push_back(directionalLight);
		}
		if (spotLight != nullptr)
		{
			m_spotLights.push_back(spotLight);
		}
		if (meshRenderer != nullptr)
		{
			m_meshRenderers.push_back(meshRenderer);
		}
		if (collider != nullptr)
		{
			m_colliders.push_back(collider);
		}
	}
}

void Editor::renderUI(Scene& scene)
{

	if (!m_isUIVisible)
		return;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("toggle visibility"))
		{
			if (ImGui::RadioButton("colliders visibility", scene.getAreCollidersVisible()))
			{
				scene.toggleColliderVisibility();
			}

			if (ImGui::RadioButton("debug deferred visibility", scene.getIsDebugDeferredVisible()))
			{
				scene.toggleDebugDeferredVisibility();
			}

			if (ImGui::RadioButton("gizmo visibility", m_isGizmoVisible))
			{
				toggleGizmoVisibility();
			}

			if (ImGui::RadioButton("light boundingBox visibility", scene.getAreLightsBoundingBoxVisible()))
			{
				scene.toggleLightsBoundingBoxVisibility();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("tools"))
		{
			if (ImGui::RadioButton("terrain tool", m_terrainToolVisible))
			{
				m_terrainToolVisible = !m_terrainToolVisible;
			}
			if (ImGui::RadioButton("skybox tool", m_skyboxToolVisible))
			{
				m_skyboxToolVisible = !m_skyboxToolVisible;
			}
			if (ImGui::RadioButton("texture factory", m_textureFactoryVisible))
			{
				m_textureFactoryVisible = !m_textureFactoryVisible;
			}
			if (ImGui::RadioButton("cube texture factory", m_cubeTextureFactoryVisible))
			{
				m_cubeTextureFactoryVisible = !m_cubeTextureFactoryVisible;
			}
			if (ImGui::RadioButton("mesh factory", m_meshFactoryVisible))
			{
				m_meshFactoryVisible = !m_meshFactoryVisible;
			}
			if (ImGui::RadioButton("program factory", m_programFactoryVisible))
			{
				m_programFactoryVisible = !m_programFactoryVisible;
			}
			if (ImGui::RadioButton("material factory", m_materialFactoryVisible))
			{
				m_materialFactoryVisible = !m_materialFactoryVisible;
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Add default entities"))
		{
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
				light->setBoundingBoxVisual(new MeshRenderer(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get("wireframe")));
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
				light->setBoundingBoxVisual(new MeshRenderer(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get("wireframe")));
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

			ImGui::EndMenu();
		}

		ImGui::Text("                                 Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::EndMainMenuBar();
	}

	int entityId = 0;

	if (!m_currentSelected.empty())
	{
		//can't add or remove components in multiple editing, only change components parameters

		ImGui::Begin("selected entities");

		if (ImGui::RadioButton("multiple editing", m_multipleEditing))
		{
			m_multipleEditing = !m_multipleEditing;

			if (m_multipleEditing)
			{
				refreshSelectedComponents(true);
			}
		}

		if (!m_multipleEditing)
		{
			for(auto& selected : m_currentSelected)
			{
				ImGui::PushID(entityId);

				if(ImGui::CollapsingHeader( ("entity "+patch::to_string(entityId)).c_str() ))
					selected->drawUI();
				ImGui::PopID();
				
					entityId++;
			}
		}
		else
		{
			m_inspector.drawUI(m_currentSelected);
			m_inspector.drawUI(m_pointLights);
			m_inspector.drawUI(m_directionlLights);
			m_inspector.drawUI(m_spotLights);
			m_inspector.drawUI(m_meshRenderers);
		}


		ImGui::End();
	}

	if (m_terrainToolVisible)
	{
		ImGui::Begin("Terrain tool");
		scene.getTerrain().drawUI();
		ImGui::End();
	}

	if (m_skyboxToolVisible)
	{
		ImGui::Begin("Skybox tool");
		scene.getSkybox().drawUI();
		ImGui::End();
	}

	if (m_textureFactoryVisible)
	{
		ImGui::Begin("Texture factory");
		TextureFactory::get().drawUI();
		ImGui::End();
	}

	if (m_cubeTextureFactoryVisible)
	{
		ImGui::Begin("Cube Texture factory");
		CubeTextureFactory::get().drawUI();
		ImGui::End();
	}

	if (m_meshFactoryVisible)
	{
		ImGui::Begin("Mesh factory");
		MeshFactory::get().drawUI();
		ImGui::End();
	}

	if (m_programFactoryVisible)
	{
		ImGui::Begin("Program factory");
		ProgramFactory::get().drawUI();
		ImGui::End();
	}

	if (m_materialFactoryVisible)
	{
		ImGui::Begin("Material factory");
		MaterialFactory::get().drawUI();
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
	scene.setAreLightsBoundingBoxVisible(m_isUIVisible);
	m_isGizmoVisible = m_isUIVisible;

}

void Editor::toggleLightsBoundingBoxVisibility(Scene& scene)
{
	scene.toggleLightsBoundingBoxVisibility();
}

void Editor::update(Camera & camera)
{
	//update gizmo
	float distanceToCamera = glm::length(camera.eye - m_gizmo->getPosition());
	m_gizmo->setScale(distanceToCamera*0.1f);
}
