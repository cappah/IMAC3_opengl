#include "Editor.h"
//forwards :
#include "Scene.h"
#include "Application.h"
#include "Factories.h"
#include "InputHandler.h"
#include "Project.h"



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
			//entity->applyTransform();
		}
	}

	vector3Value = entities[0]->getScale();
	if (ImGui::InputFloat3("scale", &vector3Value[0]))
	{
		for (auto& entity : entities)
		{
			entity->setScale(vector3Value);
			//entity->applyTransform();
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
			if (MaterialFactory::get().contains<Material3DObject>(textValue))
			{
				meshRenderer->setMaterial( MaterialFactory::get().get<Material3DObject>(textValue) );
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

Editor::Editor(MaterialUnlit* _unlitMaterial) : m_isGizmoVisible(true), m_isMovingGizmo(false), m_isUIVisible(true), m_multipleEditing(false), m_cameraFPS(true), m_cameraBaseSpeed(0.1f), m_cameraBoostSpeed(0.5f)
{
	m_savePath[0] = '\0';
	m_loadPath[0] = '\0';

	m_gizmo = new Gizmo(_unlitMaterial, this);

	m_camera = new CameraEditor();
	m_camera->setFPSMode(m_cameraFPS);
	//camera_defaults(*m_camera);

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
	glm::mat4 projectionMatrix = m_camera->getProjectionMatrix(); // glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.f);
	glm::mat4 viewMatrix = m_camera->getViewMatrix(); // glm::lookAt(m_camera->eye, m_camera->o, m_camera->up);

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

void Editor::displayMenuBar(Project& project)
{
	Scene& scene = *project.getActiveScene();

	bool saveWindowOpen = false;
	bool loadWindowOpen = false;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("options"))
		{
			if (ImGui::Selectable("save"))
			{
				if (project.getName() == "")
					saveWindowOpen = true;
				else
					project.save();
				//ImGui::OpenPopup("save window");
			}
			if (ImGui::Selectable("save as"))
			{
				saveWindowOpen = true;
			}
			if(ImGui::Selectable("load"))
			{
				loadWindowOpen = true;
				//ImGui::OpenPopup("load window");
			}

			ImGui::EndMenu();
		}
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
		if (ImGui::BeginMenu("Add default entities"))
		{
			if (ImGui::Button("add empty entity"))
			{
				auto newEntity = new Entity(&scene);
				auto newCollider = new BoxCollider(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get<MaterialUnlit>("wireframe"));
				newEntity->add(newCollider);

				newEntity->setTranslation(m_camera->getCameraPosition() + m_camera->getCameraForward()*3.f);
				changeCurrentSelected(newEntity);
			}
			ImGui::SameLine();
			if (ImGui::Button("add pointLight"))
			{
				auto newEntity = new Entity(&scene);
				auto newCollider = new BoxCollider(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get<MaterialUnlit>("wireframe"));
				auto light = new PointLight();
				light->setBoundingBoxVisual( MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get<MaterialUnlit>("wireframe"));
				newEntity->add(newCollider).add(light);
				newEntity->setName("point light");

				newEntity->setTranslation(m_camera->getCameraPosition() + m_camera->getCameraForward()*3.f);
				changeCurrentSelected(newEntity);
			}
			ImGui::SameLine();
			if (ImGui::Button("add directionalLight"))
			{
				auto newEntity = new Entity(&scene);
				auto newCollider = new BoxCollider(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get<MaterialUnlit>("wireframe"));
				auto light = new DirectionalLight();
				newEntity->add(newCollider).add(light);
				newEntity->setName("directional light");

				newEntity->setTranslation(m_camera->getCameraPosition() + m_camera->getCameraForward()*3.f);
				changeCurrentSelected(newEntity);
			}
			ImGui::SameLine();
			if (ImGui::Button("add spotLight"))
			{
				auto newEntity = new Entity(&scene);
				auto newCollider = new BoxCollider(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get<MaterialUnlit>("wireframe"));
				auto light = new SpotLight();
				light->setBoundingBoxVisual(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get<MaterialUnlit>("wireframe"));
				newEntity->add(newCollider).add(light);
				newEntity->setName("spot light");

				newEntity->setTranslation(m_camera->getCameraPosition() + m_camera->getCameraForward()*3.f);
				changeCurrentSelected(newEntity);
			}

			if (ImGui::Button("add cube"))
			{
				auto newEntity = new Entity(&scene);
				auto newCollider = new BoxCollider(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get<MaterialUnlit>("wireframe"));
				auto meshRenderer = new MeshRenderer(MeshFactory::get().get("cube"), MaterialFactory::get().get<Material3DObject>("brick"));
				newEntity->add(newCollider).add(meshRenderer);
				newEntity->setName("cube");

				newEntity->setTranslation(m_camera->getCameraPosition() + m_camera->getCameraForward()*3.f);
				changeCurrentSelected(newEntity);
			}

			if (ImGui::Button("add Camera"))
			{
				auto newEntity = new Entity(&scene);
				auto newCollider = new BoxCollider(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get<MaterialUnlit>("wireframe"));
				auto camera = new Camera();
				newEntity->add(newCollider).add(camera);
				newEntity->setName("camera");

				newEntity->setTranslation(m_camera->getCameraPosition() + m_camera->getCameraForward()*3.f);
				changeCurrentSelected(newEntity);
			}

			if (ImGui::Button("add flag"))
			{
				auto newEntity = new Entity(&scene);
				auto newCollider = new BoxCollider(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get<MaterialUnlit>("wireframe"));
				auto flag = new Physic::Flag(MaterialFactory::get().get<Material3DObject>("default"), 10);
				newEntity->add(newCollider).add(flag);
				newEntity->setName("flag");
			}

			if (ImGui::Button("add path point"))
			{
				auto newEntity = new Entity(&scene);
				auto newCollider = new BoxCollider(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get<MaterialUnlit>("wireframe"));
				auto pathPoint = new PathPoint();
				newEntity->add(newCollider).add(pathPoint);
				newEntity->setName("path point");
			}

			if (ImGui::Button("add wind zone"))
			{
				auto newEntity = new Entity(&scene);
				auto newCollider = new BoxCollider(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get<MaterialUnlit>("wireframe"));
				auto windZone = new Physic::WindZone();
				newEntity->add(newCollider).add(windZone);
				newEntity->setName("wind zone");
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("camera mode"))
		{
			if (ImGui::RadioButton("editor camera", !m_cameraFPS))
			{
				if (m_cameraFPS)
				{
					//CameraEditor* newCam = new CameraEditor();
					////newCam->switchFromCameraFPS(*m_camera); //set up the camera
					//delete m_camera;
					//m_camera = newCam;
					//toogleCamera(*m_camera);
					m_camera->setFPSMode(false);
					m_cameraFPS = false;
				}
			}
			if (ImGui::RadioButton("FPS camera", m_cameraFPS))
			{
				if (!m_cameraFPS)
				{
					//CameraFPS* newCam = new CameraFPS();
					////newCam->switchFromCameraEditor(*m_camera); //set up the camera
					//delete m_camera;
					//m_camera = newCam;
					//toogleCamera(*m_camera);
					m_camera->setFPSMode(true);
					m_cameraFPS = true;
				}
			}
			if (ImGui::RadioButton("hide cursor", m_hideCursorWhenMovingCamera))
			{
				m_hideCursorWhenMovingCamera = !m_hideCursorWhenMovingCamera;
			}
			ImGui::SliderFloat("camera base speed", &m_cameraBaseSpeed, 0.01f, 1.f);
			ImGui::SliderFloat("camera boost speed", &m_cameraBoostSpeed, 0.01f, 1.f);
			float tmpFloat = m_camera->getFOV();
			if(ImGui::SliderFloat("camera fov", &(tmpFloat), 0.f, glm::pi<float>()))
				m_camera->setFOV(tmpFloat);

			tmpFloat = m_camera->getNear();
			if (ImGui::SliderFloat("camera near", &(tmpFloat), 0.001f, 5.f))
				m_camera->setNear(tmpFloat);

			tmpFloat = m_camera->getFar();
			if (ImGui::SliderFloat("camera far", &(tmpFloat), 0.01f, 1000.f))
				m_camera->setFar(tmpFloat);

			tmpFloat = m_camera->getAspect();
			if (ImGui::SliderFloat("camera aspect", &(tmpFloat), 0.01f, 10.f))
				m_camera->setAspect(tmpFloat);

			ImGui::EndMenu();
		}

		ImGui::Text("                                 Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::EndMainMenuBar();
	}

	//modal windows : 
	if(saveWindowOpen)
		ImGui::OpenPopup("save window");
	if (loadWindowOpen)
		ImGui::OpenPopup("load window");

	//load : 
	bool loadModalWindowOpen = true;
	if (ImGui::BeginPopupModal("load window", &loadModalWindowOpen))
	{
		ImGui::InputText("project name", &m_loadPath[0], 60);
		if (ImGui::Button("load"))
		{
			std::string loadPath = ("save/" + std::string(m_loadPath));

			//Verify the validity of path :
			std::vector<std::string> dirNames = getAllDirNames("save/");
			bool dirAlreadyExists = (std::find(dirNames.begin(), dirNames.end(), std::string(m_loadPath)) != dirNames.end());

			if (m_loadPath != "" && dirAlreadyExists)
			{
				project.open(m_loadPath, loadPath);
				//scene.clear(); //clear the previous scene
				//scene.load(loadPath);
			}
			else
			{
				std::cout << "can't load project with name : " << m_loadPath << " no project found." << std::endl;
			}
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	//save
	bool saveModalWindowOpen = true;
	if (ImGui::BeginPopupModal("save window", &saveModalWindowOpen))
	{
		ImGui::InputText("set project name", m_savePath, 60);
		if (ImGui::Button("save"))
		{
			std::string savePath = ("save/"+std::string(m_savePath));

			//Verify the validity of path :
			std::vector<std::string> dirNames = getAllDirNames("save/");
			bool dirAlreadyExists = (std::find(dirNames.begin(), dirNames.end(), std::string(m_savePath)) != dirNames.end());
			
			if (m_savePath != "" && !dirAlreadyExists)
			{
				project.setName(m_savePath);
				project.setPath(savePath);
				project.save();
			}
			else
			{
				std::cout << "can't save project with name : " << m_savePath << " a project with the same name was found." << std::endl;
			}

			//scene.save(savePath);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}


}

void Editor::displayTopLeftWindow(Project& project)
{
	Scene& scene = *project.getActiveScene();

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
					selected->drawUI(scene);
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




void Editor::displayTreeEntityNode(Entity* entity, int &entityId, bool &setParenting, Entity*& parentToAttachSelected)
{
	ImGui::PushID(entityId);
	bool nodeOpen = false;

	bool isSelected = entity->getIsSelected();
	if (isSelected)
	{
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 0, 1));
		//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2, 0, 0, 1));
		//ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0.8, 0.8, 1));
	}

	ImVec2 itemPos;
	ImVec2 itemSize;
	if (ImGui::MyTreeNode( std::to_string(entityId).c_str(), itemPos, itemSize))
		nodeOpen = true;
	ImGui::SameLine();


	if (ImGui::Button(entity->getName().c_str(), ImVec2(itemSize.x /*m_bottomLeftPanelRect.z*/ - 36.f, itemSize.y /*16.f*/)))
	{
		if (isSelected)
		{
			glm::vec3 cameraFinalPosition = entity->getTranslation() - m_camera->getCameraForward()*3.f;
			m_camera->setTranslation(cameraFinalPosition);
		}
		else
			changeCurrentSelected(entity);
	}

	ImGui::SameLine();

	if (ImGui::Button("<"))
	{
		setParenting = true;
		parentToAttachSelected = entity;
	}

	if (nodeOpen)
	{
		if (isSelected)
		{
			//ImGui::PopStyleColor();
			//ImGui::PopStyleColor();
			//ImGui::PopStyleColor();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8, 0.8, 0.8, 0.2));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2, 0.2, 0.2, 0.2));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8, 0.8, 0.8, 0.2));
		}

		for (int c = 0; c < entity->getChildCount(); c++)
		{
			entityId++;
			displayTreeEntityNode(entity->getChild(c), entityId, setParenting, parentToAttachSelected);
		}
	}

	if(nodeOpen)
		ImGui::TreePop();

	if (isSelected && !nodeOpen)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8, 0.8, 0.8, 0.2));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2, 0.2, 0.2, 0.2));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8, 0.8, 0.8, 0.2));
		//ImGui::PopStyleColor();
		//ImGui::PopStyleColor();
		//ImGui::PopStyleColor();
	}

	ImGui::PopID();
	entityId++;
}

void Editor::displayBottomLeftWindow(Project& project)
{
	Scene& scene = *project.getActiveScene();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8, 0.8, 0.8, 0.2) );
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2, 0.2, 0.2, 0.2));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8, 0.8, 0.8, 0.2));

	auto entities = scene.getEntities();

	int entityId = 0;
	bool setParenting = false;
	Entity* parentToAttachSelected = nullptr;
	for (auto& entity : entities)
	{
		if (!entity->hasParent())
		{
			displayTreeEntityNode(entity, entityId, setParenting, parentToAttachSelected);
		}
	}

	if (setParenting)
	{
		if (parentToAttachSelected->getIsSelected())
			parentToAttachSelected->setParent(nullptr);
		else
		{
			for (int i = 0; i < m_currentSelected.size(); i++)
			{
				m_currentSelected[i]->setParent(parentToAttachSelected);
			}
		}
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

void Editor::displayBottomWindow(Project& project)
{
	Scene& scene = *project.getActiveScene();

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

void Editor::renderUI(Project& project)
{
		
	Scene& scene = *project.getActiveScene();

	if (!m_isUIVisible)
		return;

	displayMenuBar(project);


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
				displayTopLeftWindow(project);
			ImGui::EndChild();


				//ImGui::InvisibleButton("hSplitter0", ImVec2(m_topLeftPanelRect.z, 8.f));
				//if (ImGui::IsItemActive())
				//{
				//	m_topLeftPanelRect.w += ImGui::GetIO().MouseDelta.y;
				//	if (m_topLeftPanelRect.w < 10) m_topLeftPanelRect.w = 10;
				//	else if (m_topLeftPanelRect.w > m_windowRect.w - 20) m_topLeftPanelRect.w = m_topLeftPanelRect.w - 20;
				//	updatePanelSize(m_topLeftPanelRect.z, m_topLeftPanelRect.w, m_bottomPanelRect.w);
				//}
			ImGui::Separator();
				ImGui::InvisibleButton("hSplitter1", ImVec2(m_topLeftPanelRect.z, 8.f));
				if (ImGui::IsItemActive())
				{
					m_topLeftPanelRect.w += ImGui::GetIO().MouseDelta.y;
					if (m_topLeftPanelRect.w < 10) m_topLeftPanelRect.w = 10;
					else if (m_topLeftPanelRect.w > m_windowRect.w - 20) m_topLeftPanelRect.w = m_topLeftPanelRect.w - 20;
					updatePanelSize(m_topLeftPanelRect.z, m_topLeftPanelRect.w, m_bottomPanelRect.w);
				}
			ImGui::Separator();

			ImGui::BeginChild("bottomLeftWindowContent", ImVec2(m_topLeftPanelRect.z - 30, m_bottomLeftPanelRect.w - 16.f));
				displayBottomLeftWindow(project);
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
	//ImGui::Separator();
	if (ImGui::IsItemActive())
	{
		m_bottomPanelRect.w -= ImGui::GetIO().MouseDelta.y;
		if (m_bottomPanelRect.w < 10) m_bottomPanelRect.w = 10;
		else if (m_bottomPanelRect.w > m_windowRect.w - 20) m_bottomPanelRect.w = m_bottomPanelRect.w - 20;
		updatePanelSize(m_topLeftPanelRect.z, m_topLeftPanelRect.w, m_bottomPanelRect.w);
	}

	ImGui::BeginChild("bottomWindowContent");
		displayBottomWindow(project);
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
				m_camera->translateLocal(glm::vec3(0, 0, GUIStates::MOUSE_ZOOM_SPEED * zoomDir));
				//camera_zoom(*m_camera, zoomDir * GUIStates::MOUSE_ZOOM_SPEED);
			}
			else if (m_guiStates.turnLock)
			{
				m_camera->rotate(diffLockPositionX * GUIStates::MOUSE_TURN_SPEED, diffLockPositionY * GUIStates::MOUSE_TURN_SPEED);

				//camera_turn(*m_camera, diffLockPositionY * GUIStates::MOUSE_TURN_SPEED,
				//	diffLockPositionX * GUIStates::MOUSE_TURN_SPEED);

			}
		}
		if (m_guiStates.altPressed == GLFW_PRESS && m_guiStates.shiftPressed == GLFW_PRESS)
		{
			if (m_guiStates.panLock)
			{
				m_camera->translateLocal( glm::vec3(diffLockPositionX * GUIStates::MOUSE_PAN_SPEED, diffLockPositionY * GUIStates::MOUSE_PAN_SPEED, 0));

/*				camera_pan(*m_camera, diffLockPositionX * GUIStates::MOUSE_PAN_SPEED,
					diffLockPositionY * GUIStates::MOUSE_PAN_SPEED)*/;
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

		m_camera->rotate(diffLockPositionX * GUIStates::MOUSE_TURN_SPEED, diffLockPositionY * GUIStates::MOUSE_TURN_SPEED);

		//camera_rotate(*m_camera, diffLockPositionY * GUIStates::MOUSE_TURN_SPEED,
		//	diffLockPositionX * GUIStates::MOUSE_TURN_SPEED);

		glfwSetCursorPos(window, m_guiStates.lockPositionX, m_guiStates.lockPositionY);
	}

	if (m_guiStates.altPressed || m_guiStates.ctrlPressed)
		return;

	if (!m_guiStates.UICaptureKeyboard)
	{
		float cameraSpeed = m_cameraBaseSpeed;
		if (m_guiStates.shiftPressed)
			cameraSpeed = m_cameraBoostSpeed;

		glm::vec3 translateDirection = glm::vec3(0,0,0);

		if (m_guiStates.leftPressed)
			translateDirection += glm::vec3(-1, 0, 0);
		if (m_guiStates.rightPressed)
			translateDirection += glm::vec3(1, 0, 0);
		if (m_guiStates.forwardPressed)
			translateDirection += glm::vec3(0, 0, 1);
		if (m_guiStates.backwardPressed)
			translateDirection += glm::vec3(0, 0, -1);

		if (translateDirection != glm::vec3(0, 0, 0))
		{
			translateDirection = glm::normalize(translateDirection) * cameraSpeed;
			m_camera->translateLocal(translateDirection);
		}


		//if (m_guiStates.leftPressed)
		//	camera_translate(*m_camera, -cameraSpeed, 0, 0);
		//if (m_guiStates.rightPressed)
		//	camera_translate(*m_camera, cameraSpeed, 0, 0);
		//if (m_guiStates.forwardPressed)
		//	camera_translate(*m_camera, 0, 0, cameraSpeed);
		//if (m_guiStates.backwardPressed)
		//	camera_translate(*m_camera, 0, 0, -cameraSpeed);
	}
}


void Editor::update(/*Camera & camera*/ Scene& scene, GLFWwindow* window)
{
	float screenWidth = Application::get().getWindowWidth();
	float screenHeight = Application::get().getWindowHeight();

	//update tools : 
	if (m_terrainToolVisible) // for terrain
	{
		if (InputHandler::getMouseButton(GLFW_MOUSE_BUTTON_1) && !m_guiStates.mouseOverUI && !m_guiStates.altPressed && !m_guiStates.ctrlPressed && !m_guiStates.shiftPressed)
		{
			glm::vec3 origin = m_camera->getCameraPosition();
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			glm::vec3 direction = screenToWorld(mouseX, mouseY, screenWidth, screenHeight, *m_camera);

			Ray ray(origin, direction, 1000.f);
			CollisionInfo collisionInfo;

			if (ray.intersectTerrain(scene.getTerrain(), collisionInfo))
			{
				if(scene.getTerrain().getCurrentTerrainTool() == Terrain::DRAW_MATERIAL)
					scene.getTerrain().drawMaterialOnTerrain(collisionInfo.point);
				else if(scene.getTerrain().getCurrentTerrainTool() == Terrain::DRAW_GRASS)
					scene.getTerrain().drawGrassOnTerrain(collisionInfo.point);
			}
		}
	}

	//update gizmo
	float distanceToCamera = glm::length(m_camera->getCameraPosition() - m_gizmo->getPosition());
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
		if (InputHandler::getKeyDown(GLFW_KEY_TAB) && m_guiStates.ctrlPressed)
		{
			this->toggleDebugVisibility(scene);
		}

		//entity copy / past : 
		if (InputHandler::getKeyDown(GLFW_KEY_D) && m_guiStates.ctrlPressed)
		{
			this->duplicateSelected();
		}

		//delete selected : 
		if (InputHandler::getKeyDown(GLFW_KEY_DELETE))
		{
			this->deleteSelected(scene);
		}

	}

	if (!m_guiStates.mouseOverUI)
	{
		//object picking : 
		if (!m_guiStates.altPressed && !m_guiStates.ctrlPressed
			&& InputHandler::getMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
		{

			glm::vec3 origin = m_camera->getCameraPosition();
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

	if (InputHandler::getMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT))
	{
		if (this->isMovingGizmo())
			this->endMoveGizmo();
	}
	if (InputHandler::getMouseButton(GLFW_MOUSE_BUTTON_LEFT))
	{
		if (this->isMovingGizmo())
		{
			float screenWidth = Application::get().getWindowWidth();
			float screenHeight = Application::get().getWindowHeight();

			glm::vec3 origin = m_camera->getCameraPosition();
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			glm::vec3 direction = screenToWorld(mouseX, mouseY, screenWidth, screenHeight, *m_camera);
			Ray ray(origin, direction, 1000.f);

			this->moveGizmo(ray);
		}
	}
	
}

CameraEditor& Editor::getCamera()
{
	return *m_camera;
}

