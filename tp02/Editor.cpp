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
		floatValue = pointLights[0]->getIntensity();
		if (ImGui::SliderFloat("light intensity", &floatValue, 0.f, 50.f))
		{
			for (auto& light : pointLights)
			{
				light->setIntensity( floatValue );
			}
		}
		vector3Value = pointLights[0]->getColor();
		if (ImGui::ColorEdit3("light color", &vector3Value[0]))
		{
			for (auto& light : pointLights)
			{
				light->setColor( vector3Value );
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
		floatValue = directionalLights[0]->getIntensity();
		if (ImGui::SliderFloat("light intensity", &floatValue, 0.f, 10.f))
		{
			for (auto& light : directionalLights)
			{
				light->setIntensity( floatValue );
			}
		}
		vector3Value = directionalLights[0]->getColor();
		if (ImGui::ColorEdit3("light color", &vector3Value[0]))
		{
			for (auto& light : directionalLights)
			{
				light->setColor( vector3Value );
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
		floatValue = spotLights[0]->getIntensity();
		if (ImGui::SliderFloat("light intensity", &floatValue, 0.f, 50.f))
		{
			for (auto& light : spotLights)
			{
				light->setIntensity( floatValue );
			}
		}
		vector3Value = spotLights[0]->getColor();
		if (ImGui::ColorEdit3("light color", &vector3Value[0]))
		{
			for (auto& light : spotLights)
			{
				light->setColor( vector3Value );
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



////////////////////////////////////////// GUI STATES

const float GUIStates::MOUSE_PAN_SPEED = 0.001f;
const float GUIStates::MOUSE_ZOOM_SPEED = 0.05f;
const float GUIStates::MOUSE_TURN_SPEED = 0.005f;

void init_gui_states(GUIStates & guiStates)
{
	guiStates.panLock = false;
	guiStates.turnLock = false;
	guiStates.zoomLock = false;
	guiStates.lockPositionX = 0;
	guiStates.lockPositionY = 0;
	guiStates.camera = 0;
	guiStates.time = 0.0;
	guiStates.playing = false;
}




/////////////////////////////////// EDITOR

Editor::Editor(MaterialUnlit* _unlitMaterial) : m_isGizmoVisible(true), m_isMovingGizmo(false), m_isUIVisible(true), m_multipleEditing(false), m_cameraFPS(false)
{
	m_gizmo = new Gizmo(_unlitMaterial, this);

	m_camera = new CameraEditor();
	camera_defaults(*m_camera);

	init_gui_states(m_guiStates);

	//ui : 
	m_windowDecal = glm::vec2(1, 20);
	m_windowRect = glm::vec4(m_windowDecal.x, m_windowDecal.y, Application::get().getWindowWidth(), Application::get().getWindowHeight());
	m_topLeftPanelRect = glm::vec4(m_windowRect.x, m_windowRect.y, 100, 200);
	m_bottomLeftPanelRect = glm::vec4(m_windowRect.x,m_topLeftPanelRect.w, m_topLeftPanelRect.z, m_windowRect.w - m_topLeftPanelRect.w);
	m_bottomPanelRect = glm::vec4(m_windowRect.x + m_topLeftPanelRect.z, 200, m_windowRect.z - m_topLeftPanelRect.z, m_windowRect.w - 200 );

	//defaults : 
	m_terrainToolVisible = true;
	m_skyboxToolVisible = false;
	m_textureFactoryVisible = false;
	m_cubeTextureFactoryVisible = false;
	m_meshFactoryVisible = false;
	m_programFactoryVisible = false;
	m_materialFactoryVisible = false;
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

void Editor::renderGizmo()
{
	if (!m_isGizmoVisible)
		return;

	int width = Application::get().getWindowWidth(), height = Application::get().getWindowHeight();
	glm::mat4 projectionMatrix = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.f);
	glm::mat4 viewMatrix = glm::lookAt(m_camera->eye, m_camera->o, m_camera->up);

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

void Editor::hideAllToolsUI()
{
	m_terrainToolVisible = false;
	m_skyboxToolVisible = false;
	m_textureFactoryVisible = false;
	m_cubeTextureFactoryVisible = false;
	m_meshFactoryVisible = false;
	m_programFactoryVisible = false;
	m_materialFactoryVisible = false;
}

void Editor::displayMenuBar(Scene& scene)
{
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
		if (ImGui::BeginMenu("camera mode"))
		{
			if (ImGui::RadioButton("editor camera", !m_cameraFPS))
			{
				if (m_cameraFPS)
				{
					CameraEditor* newCam = new CameraEditor();
					newCam->switchFromCameraFPS(*m_camera); //set up the camera
					delete m_camera;
					m_camera = newCam;
					//toogleCamera(*m_camera);
					m_cameraFPS = false;
				}
			}
			if (ImGui::RadioButton("FPS camera", m_cameraFPS))
			{
				if (!m_cameraFPS)
				{
					CameraFPS* newCam = new CameraFPS();
					newCam->switchFromCameraEditor(*m_camera); //set up the camera
					delete m_camera;
					m_camera = newCam;
					//toogleCamera(*m_camera);
					m_cameraFPS = true;
				}
			}
			if (ImGui::RadioButton("hide cursor", m_hideCursorWhenMovingCamera))
			{
				m_hideCursorWhenMovingCamera = !m_hideCursorWhenMovingCamera;
			}

			ImGui::EndMenu();
		}

		ImGui::Text("                                 Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::EndMainMenuBar();
	}


}

void Editor::displayTopLeftWindow(Scene& scene)
{

	int entityId = 0;

	if (!m_currentSelected.empty())
	{
		//can't add or remove components in multiple editing, only change components parameters
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
			for (auto& selected : m_currentSelected)
			{
				ImGui::PushID(entityId);

				if (ImGui::CollapsingHeader(("entity " + patch::to_string(entityId)).c_str()))
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
	}
}

void Editor::displayBottomLeftWindow(Scene& scene)
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4, 0.4, 0.4, 0.2));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2, 0.2, 0.2, 0.2));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8, 0.8, 0.8, 0.2));

	auto entities = scene.getEntities();

	int entityId = 0;
	for (auto& entity : entities)
	{
		ImGui::PushID(entityId);
		if (ImGui::Button(entity->getName().c_str(), ImVec2(m_bottomLeftPanelRect.z - 35.f, 16.f)))
		{
			glm::vec3 cameraFinalPosition = entity->getTranslation() - m_camera->dir*3.f;
			m_camera->setTranslation(cameraFinalPosition);
			m_camera->setDirection(entity->getTranslation() - m_camera->eye);
		}
		ImGui::PopID();
		entityId++;
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

void Editor::displayBottomWindow(Scene& scene)
{
	ImGui::BeginChild("choose tool", ImVec2(200, ImGui::GetWindowHeight()));
	if (ImGui::RadioButton("terrain tool", m_terrainToolVisible))
	{
		hideAllToolsUI();
		m_terrainToolVisible = true;
	}
	if (ImGui::RadioButton("skybox tool", m_skyboxToolVisible))
	{
		hideAllToolsUI();
		m_skyboxToolVisible = true;
	}
	if (ImGui::RadioButton("texture factory", m_textureFactoryVisible))
	{
		hideAllToolsUI();
		m_textureFactoryVisible = true;
	}
	if (ImGui::RadioButton("cube texture factory", m_cubeTextureFactoryVisible))
	{
		hideAllToolsUI();
		m_cubeTextureFactoryVisible = true;
	}
	if (ImGui::RadioButton("mesh factory", m_meshFactoryVisible))
	{
		hideAllToolsUI();
		m_meshFactoryVisible = true;
	}
	if (ImGui::RadioButton("program factory", m_programFactoryVisible))
	{
		hideAllToolsUI();
		m_programFactoryVisible = true;
	}
	if (ImGui::RadioButton("material factory", m_materialFactoryVisible))
	{
		hideAllToolsUI();
		m_materialFactoryVisible = true;
	}
	ImGui::EndChild();
	
	ImGui::SameLine();

	if (m_terrainToolVisible)
	{
		ImGui::BeginChild("Terrain tool");
		scene.getTerrain().drawUI();
		ImGui::End();
	}

	if (m_skyboxToolVisible)
	{
		ImGui::BeginChild("Skybox tool");
		scene.getSkybox().drawUI();
		ImGui::End();
	}

	if (m_textureFactoryVisible)
	{
		ImGui::BeginChild("Texture factory");
		TextureFactory::get().drawUI();
		ImGui::End();
	}

	if (m_cubeTextureFactoryVisible)
	{
		ImGui::BeginChild("Cube Texture factory");
		CubeTextureFactory::get().drawUI();
		ImGui::End();
	}

	if (m_meshFactoryVisible)
	{
		ImGui::BeginChild("Mesh factory");
		MeshFactory::get().drawUI();
		ImGui::End();
	}

	if (m_programFactoryVisible)
	{
		ImGui::BeginChild("Program factory");
		ProgramFactory::get().drawUI();
		ImGui::End();
	}

	if (m_materialFactoryVisible)
	{
		ImGui::BeginChild("Material factory");
		MaterialFactory::get().drawUI();
		ImGui::End();
	}
}

void Editor::updatePanelSize(float topLeftWidth, float topLeftHeight, float bottomHeight)
{
	m_windowRect = glm::vec4(m_windowDecal.x, m_windowDecal.y, Application::get().getWindowWidth(), Application::get().getWindowHeight());
	m_topLeftPanelRect = glm::vec4(m_windowRect.x, m_windowRect.y, topLeftWidth, topLeftHeight);
	m_bottomLeftPanelRect = glm::vec4(m_windowRect.x, m_topLeftPanelRect.w, m_topLeftPanelRect.z, m_windowRect.w - m_topLeftPanelRect.w);
	m_bottomPanelRect = glm::vec4(m_windowRect.x + m_topLeftPanelRect.z, m_windowRect.w - bottomHeight, m_windowRect.z - m_topLeftPanelRect.z, bottomHeight);
}

void Editor::onResizeWindow()
{
	m_windowRect.z = Application::get().getWindowWidth();
	m_windowRect.w = Application::get().getWindowHeight();

	updatePanelSize(m_topLeftPanelRect.z, m_topLeftPanelRect.w, m_bottomPanelRect.w);
}

void Editor::renderUI(Scene& scene)
{

	if (!m_isUIVisible)
		return;

	displayMenuBar(scene);


	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1, 0.1, 0.1, 255));

	//top left window :
	ImGui::SetNextWindowSize(ImVec2(m_topLeftPanelRect.z, m_windowRect.w));
	ImGui::SetNextWindowContentSize(ImVec2(m_topLeftPanelRect.z, m_windowRect.w));
	ImGui::SetNextWindowPos(ImVec2(m_topLeftPanelRect.x, m_topLeftPanelRect.y));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("leftWindow", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_ShowBorders);
		ImGui::BeginChild("leftWindowContent", ImVec2(m_topLeftPanelRect.z -30, m_windowRect.w));

			ImGui::BeginChild("topLeftWindowContent", ImVec2(m_topLeftPanelRect.z - 30, m_topLeftPanelRect.w - 16.f));
				displayTopLeftWindow(scene);
			ImGui::EndChild();


				ImGui::InvisibleButton("hSplitter0", ImVec2(m_topLeftPanelRect.z, 8.f));
				if (ImGui::IsItemActive())
				{
					m_topLeftPanelRect.w += ImGui::GetIO().MouseDelta.y;
					if (m_topLeftPanelRect.w < 10) m_topLeftPanelRect.w = 10;
					else if (m_topLeftPanelRect.w > m_windowRect.w - 20) m_topLeftPanelRect.w = m_topLeftPanelRect.w - 20;
					updatePanelSize(m_topLeftPanelRect.z, m_topLeftPanelRect.w, m_bottomPanelRect.w);
				}
			ImGui::Separator();
				ImGui::InvisibleButton("hSplitter1", ImVec2(m_topLeftPanelRect.z, 8.f));
				if (ImGui::IsItemActive())
				{
					m_topLeftPanelRect.w += ImGui::GetIO().MouseDelta.y;
					if (m_topLeftPanelRect.w < 10) m_topLeftPanelRect.w = 10;
					else if (m_topLeftPanelRect.w > m_windowRect.w - 20) m_topLeftPanelRect.w = m_topLeftPanelRect.w - 20;
					updatePanelSize(m_topLeftPanelRect.z, m_topLeftPanelRect.w, m_bottomPanelRect.w);
				}

			ImGui::BeginChild("bottomLeftWindowContent", ImVec2(m_topLeftPanelRect.z - 30, m_bottomLeftPanelRect.w - 16.f));
				displayBottomLeftWindow(scene);
			ImGui::EndChild();

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::InvisibleButton("vSplitter", ImVec2(20.f, m_windowRect.w));
		if (ImGui::IsItemActive())
		{
			m_topLeftPanelRect.z += ImGui::GetIO().MouseDelta.x;
			if (m_topLeftPanelRect.z < 10) m_topLeftPanelRect.z = 10;
			else if (m_topLeftPanelRect.z > m_windowRect.z - 10) m_topLeftPanelRect.z = m_windowRect.z - 10;
			updatePanelSize(m_topLeftPanelRect.z, m_topLeftPanelRect.w, m_bottomPanelRect.w);
		}

	ImGui::End();
	ImGui::PopStyleVar();

	//bottom window :
	ImGui::SetNextWindowSize(ImVec2(m_bottomPanelRect.z, m_bottomPanelRect.w));
	ImGui::SetNextWindowContentSize(ImVec2(m_bottomPanelRect.z, m_bottomPanelRect.w));
	ImGui::SetNextWindowPos(ImVec2(m_bottomPanelRect.x, m_bottomPanelRect.y));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("bottomWindow", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_ShowBorders);
	
	ImGui::InvisibleButton("hsplitter", ImVec2(m_bottomPanelRect.z, 20.f));
	if (ImGui::IsItemActive())
	{
		m_bottomPanelRect.w -= ImGui::GetIO().MouseDelta.y;
		if (m_bottomPanelRect.w < 10) m_bottomPanelRect.w = 10;
		else if (m_bottomPanelRect.w > m_windowRect.w - 20) m_bottomPanelRect.w = m_bottomPanelRect.w - 20;
		updatePanelSize(m_topLeftPanelRect.z, m_topLeftPanelRect.w, m_bottomPanelRect.w);
	}

	ImGui::BeginChild("bottomWindowContent");
		displayBottomWindow(scene);
	ImGui::EndChild();

	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::PopStyleVar();
	ImGui::PopStyleColor();

	//ImGui::BeginChild("bottomWindow", ImVec2(screenWidth, screenHeight - m_leftPanelHeight));
	//ImGui::EndChild();

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

void Editor::updateGuiStates(GLFWwindow* window)
{
	ImGuiIO& io = ImGui::GetIO();
	m_guiStates.mouseOverUI = io.WantCaptureMouse;
	m_guiStates.UICaptureKeyboard = io.WantCaptureKeyboard;

	// Mouse states
	m_guiStates.leftButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	m_guiStates.rightButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	m_guiStates.middleButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);

	m_guiStates.altPressed = glfwGetKey(window, GLFW_KEY_LEFT_ALT);
	m_guiStates.shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	m_guiStates.ctrlPressed = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);

	m_guiStates.leftPressed = (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS);
	m_guiStates.rightPressed = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
	m_guiStates.forwardPressed = (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS);
	m_guiStates.backwardPressed = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);

	if (m_guiStates.leftButton == GLFW_PRESS)
		m_guiStates.turnLock = true;
	else
		m_guiStates.turnLock = false;

	if (m_guiStates.rightButton == GLFW_PRESS)
		m_guiStates.zoomLock = true;
	else
		m_guiStates.zoomLock = false;

	if (m_guiStates.middleButton == GLFW_PRESS || (m_guiStates.leftButton == GLFW_PRESS && m_guiStates.altPressed))
		m_guiStates.panLock = true;
	else
		m_guiStates.panLock = false;
}

void Editor::updateCameraMovement_editor(GLFWwindow* window)
{
	// Camera movements
	if(glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL && !m_hideCursorWhenMovingCamera)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (!m_guiStates.altPressed && (m_guiStates.leftButton == GLFW_PRESS || m_guiStates.rightButton == GLFW_PRESS || m_guiStates.middleButton == GLFW_PRESS))
	{
		double x; double y;
		glfwGetCursorPos(window, &x, &y);
		m_guiStates.lockPositionX = x;
		m_guiStates.lockPositionY = y;
	}
	if (m_guiStates.altPressed == GLFW_PRESS || m_guiStates.shiftPressed == GLFW_PRESS)
	{
		if(m_hideCursorWhenMovingCamera)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		double mousex; double mousey;
		glfwGetCursorPos(window, &mousex, &mousey);
		int diffLockPositionX = mousex - m_guiStates.lockPositionX;
		int diffLockPositionY = mousey - m_guiStates.lockPositionY;

		if (m_guiStates.altPressed == GLFW_PRESS && m_guiStates.shiftPressed == GLFW_RELEASE)
		{
			if (m_guiStates.zoomLock)
			{
				float zoomDir = 0.0;
				if (diffLockPositionX > 0)
					zoomDir = -1.f;
				else if (diffLockPositionX < 0)
					zoomDir = 1.f;
				camera_zoom(*m_camera, zoomDir * GUIStates::MOUSE_ZOOM_SPEED);
			}
			else if (m_guiStates.turnLock)
			{
				camera_turn(*m_camera, diffLockPositionY * GUIStates::MOUSE_TURN_SPEED,
					diffLockPositionX * GUIStates::MOUSE_TURN_SPEED);

			}
		}
		if (m_guiStates.altPressed == GLFW_PRESS && m_guiStates.shiftPressed == GLFW_PRESS)
		{
			if (m_guiStates.panLock)
			{
				camera_pan(*m_camera, diffLockPositionX * GUIStates::MOUSE_PAN_SPEED,
					diffLockPositionY * GUIStates::MOUSE_PAN_SPEED);
			}
		}

		m_guiStates.lockPositionX = mousex;
		m_guiStates.lockPositionY = mousey;
	}
}

void Editor::updateCameraMovement_fps(GLFWwindow* window)
{
	if (!m_guiStates.rightButton)
	{
		double x; double y;
		glfwGetCursorPos(window, &x, &y);
		m_guiStates.lockPositionX = x;
		m_guiStates.lockPositionY = y;
	}
	else
	{
		if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL && !m_hideCursorWhenMovingCamera)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		double mousex; double mousey;
		glfwGetCursorPos(window, &mousex, &mousey);
		int diffLockPositionX = mousex - m_guiStates.lockPositionX;
		int diffLockPositionY = mousey - m_guiStates.lockPositionY;

		camera_rotate(*m_camera, diffLockPositionY * GUIStates::MOUSE_TURN_SPEED,
			diffLockPositionX * GUIStates::MOUSE_TURN_SPEED);

		glfwSetCursorPos(window, m_guiStates.lockPositionX, m_guiStates.lockPositionY);
	}

	if (m_guiStates.altPressed || m_guiStates.ctrlPressed)
		return;

	if (!m_guiStates.UICaptureKeyboard)
	{
		float cameraSpeed = 0.01f;
		if (m_guiStates.shiftPressed)
			cameraSpeed = 0.04f;

		if (m_guiStates.leftPressed)
			camera_translate(*m_camera, -cameraSpeed, 0, 0);
		if (m_guiStates.rightPressed)
			camera_translate(*m_camera, cameraSpeed, 0, 0);
		if (m_guiStates.forwardPressed)
			camera_translate(*m_camera, 0, 0, cameraSpeed);
		if (m_guiStates.backwardPressed)
			camera_translate(*m_camera, 0, 0, -cameraSpeed);
	}
}


void Editor::update(/*Camera & camera*/ Scene& scene, GLFWwindow* window, InputHandler& inputHandler )
{
	//update gizmo
	float distanceToCamera = glm::length(m_camera->eye - m_gizmo->getPosition());
	m_gizmo->setScale(distanceToCamera*0.1f);

	updateGuiStates(window);

	//camera movements : 
	

		if (!m_cameraFPS)
			updateCameraMovement_editor(window);
		else
			updateCameraMovement_fps(window);

	if (!m_guiStates.UICaptureKeyboard)
	{
		// ui visibility : 
		if (inputHandler.getKeyDown(window, GLFW_KEY_TAB) && m_guiStates.ctrlPressed)
		{
			this->toggleDebugVisibility(scene);
		}

		//entity copy / past : 
		if (inputHandler.getKeyDown(window, GLFW_KEY_D) && m_guiStates.ctrlPressed)
		{
			this->duplicateSelected();
		}

		//delete selected : 
		if (inputHandler.getKeyDown(window, GLFW_KEY_DELETE))
		{
			this->deleteSelected(scene);
		}

	}

	if (!m_guiStates.mouseOverUI)
	{
		//object picking : 
		if (!m_guiStates.altPressed && !m_guiStates.ctrlPressed
			&& inputHandler.getMouseButtonDown(window, GLFW_MOUSE_BUTTON_LEFT))
		{
			float screenWidth = Application::get().getWindowWidth();
			float screenHeight = Application::get().getWindowHeight();

			glm::vec3 origin = m_camera->eye;
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			glm::vec3 direction = screenToWorld(mouseX, mouseY, screenWidth, screenHeight, *m_camera);
			//direction = direction - origin;
			//direction = glm::normalize(direction);

			Ray ray(origin, direction, 1000.f);

			// intersection with gizmo
			if (this->testGizmoIntersection(ray))
			{
				this->beginMoveGizmo();
			}
			//intersection with a collider in the scene
			else
			{
				auto entities = scene.getEntities();
				float distanceToIntersection = 0;
				float minDistanceToIntersection = 0;
				Entity* selectedEntity = nullptr;
				for (int i = 0, intersectedCount = 0; i < entities.size(); i++)
				{
					Collider* collider = static_cast<Collider*>(entities[i]->getComponent(Component::ComponentType::COLLIDER));
					if (entities[i]->getComponent(Component::ComponentType::COLLIDER) != nullptr)
					{
						if (ray.intersect(*collider, &distanceToIntersection))
						{
							if (intersectedCount == 0 || distanceToIntersection < minDistanceToIntersection)
							{
								selectedEntity = entities[i];
								minDistanceToIntersection = distanceToIntersection;
							}
							intersectedCount++;

							//std::cout << "intersect a cube !!!" << std::endl;
							//ray.debugLog();
						}
					}
				}

				if (selectedEntity != nullptr)
				{
					if (!m_guiStates.shiftPressed)
						this->changeCurrentSelected(selectedEntity);
					else
						this->toggleCurrentSelected(selectedEntity);
				}
			}

		}

	}

	if (inputHandler.getMouseButtonUp(window, GLFW_MOUSE_BUTTON_LEFT))
	{
		if (this->isMovingGizmo())
			this->endMoveGizmo();
	}
	if (inputHandler.getMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
	{
		if (this->isMovingGizmo())
		{
			float screenWidth = Application::get().getWindowWidth();
			float screenHeight = Application::get().getWindowHeight();

			glm::vec3 origin = m_camera->eye;
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			glm::vec3 direction = screenToWorld(mouseX, mouseY, screenWidth, screenHeight, *m_camera);
			Ray ray(origin, direction, 1000.f);

			this->moveGizmo(ray);
		}
	}
	
}

Camera& Editor::getCamera()
{
	return *m_camera;
}

