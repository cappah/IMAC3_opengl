#include "stdafx.h"

#include "Project.h"
#include "Application.h"
#include "InputHandler.h"
#include "Factories.h"
#include "Scene.h"
#include "Editor.h"
#include "FileHandler.h"
//forwards : 
#include "DebugDrawer.h"
#include "Renderer.h"

void onWindowResize(GLFWwindow* window, int width, int height)
{
	Application::get().setWindowResize(true);
	Application::get().setWindowWidth(width);
	Application::get().setWindowHeight(height);
}

void onFilesDrop(GLFWwindow* window, int count, const char** paths)
{
	Editor::instance().onFilesDropped(count, paths);
}

//statics : 
FileHandler::Path Project::m_projectPath = FileHandler::Path();
FileHandler::Path Project::m_assetFolderPath = FileHandler::Path();
FileHandler::Path Project::m_shaderFolderPath = FileHandler::Path();
////

Project::Project() : m_activeSceneName(""), m_renderer(nullptr), m_activeScene(nullptr)
{
	m_projectPath = FileHandler::Path();
	m_newSceneName[0] = '\0';
}

Project::~Project()
{
	clear();
}

void Project::init()
{
	//int width = 1024, height= 768;
	int width = 1024, height = 680;
	float widthf = (float)width, heightf = (float)height;
	double t;
	float fps = 0.f;

	///////////////////// SET APPLICATION GLOBAL PARAMETERS /////////////////////
	Application::get().setWindowWidth(width);
	Application::get().setWindowHeight(height);

	GLFWwindow* window = initGLFW(width, height);
	Application::get().setActiveWindow(window);
	InputHandler::attachToWindow(window);
	initGlew();
	setupWindow(window);

	//initProject(); //TODO 10

	//add default assets to factories : 
	//initDefaultAssets();

	// Init viewer structures
	//Camera camera;
	//camera_defaults(camera);
	//GUIStates guiStates;
	//init_gui_states(guiStates);



	//// create and initialize our light manager
	//LightManager* lightManager = new LightManager();

	//// renderer : 
	//m_renderer = new Renderer(lightManager, "aogl.vert", "aogl_gPass.frag", "aogl_lightPass.vert", "aogl_lightPass_pointLight.frag", "aogl_lightPass_directionalLight.frag", "aogl_lightPass_spotLight.frag"); // call lightManager.init()
	//m_renderer->initPostProcessQuad("blit.vert", "blit.frag");
	//m_renderer->initialyzeShadowMapping("shadowPass.vert", "shadowPass.frag", "shadowPassOmni.vert", "shadowPassOmni.frag", "shadowPassOmni.geom");

	// Our scene : 
	//Scene scene(renderer);
}

void Project::clear()
{
	m_projectPath = FileHandler::Path();
	m_activeSceneName = "";

	//clear scene :
	if(m_activeScene != nullptr)
		delete m_activeScene;
	m_activeScene = nullptr;
	
	m_scenes.clear();
	m_scenesStatus.clear();

	//clear systems : 
	if(m_renderer != nullptr)
		delete m_renderer;

	//clear all resources : 
	clearAllResourceFactories();
}

void Project::open(const std::string & projectName, const FileHandler::Path & projectPath)
{
	clear(); //clear the current project (scenes + resources + systems)

	m_projectPath = FileHandler::Path(projectPath.toString() + "/" + projectName);
	m_assetFolderPath = FileHandler::Path(projectPath.toString() + "/" + projectName + "/assets/");
	assert(FileHandler::directoryExists(m_assetFolderPath));
	m_shaderFolderPath = FileHandler::Path(projectPath.toString() + "/" + projectName + "/shaders/");
	assert(FileHandler::directoryExists(m_shaderFolderPath));

	//we have to set projectPath before calling initProject
	initProject(); //init systems and resources

	//load the project.
	if(FileHandler::directoryExists(m_projectPath))
		load();

	//create a default sceen if there are no scene in the project.
	if (m_scenes.size() == 0)
	{
		m_activeScene = new Scene(m_renderer);
		loadDefaultScene(m_activeScene);
	}
}

void Project::saveProjectInfos()
{
	Json::Value rootProject;
	rootProject["sceneCount"] = m_scenes.size();
	int sceneIdx = 0;
	for (auto& it = m_scenes.begin(); it != m_scenes.end(); it++)
	{
		rootProject["sceneInfos"][sceneIdx]["name"] = it->first;
		rootProject["sceneInfos"][sceneIdx]["path"] = it->second.toString();
		rootProject["sceneInfos"][sceneIdx]["status"] = m_scenesStatus[it->first];
		sceneIdx++;
	}
	rootProject["activeSceneName"] = m_activeSceneName;

	std::ofstream streamProject;
	streamProject.open(m_projectPath.toString() + "/projectInfos.txt");
	if (!streamProject.is_open())
	{
		std::cout << "error, can't save project infos at path : " << m_projectPath.toString() << std::endl;
		return;
	}
	streamProject << rootProject;
}


void Project::save()
{
	FileHandler::CompletePath activeScenePath(m_projectPath.toString() + "scenes/", m_activeScene->getName(), ".txt");
	FileHandler::CompletePath resourcesPath(m_projectPath, "/resources", ".txt");

	addDirectories(FileHandler::Path(m_projectPath.toString() + "scenes/"));
	addDirectories(m_projectPath);

	std::cout << "begin project save" << std::endl;
	std::cout << "active scene path : " << activeScenePath.toString() << std::endl;
	std::cout << "resources path : " << resourcesPath.toString() << std::endl;

	//save project infos : 
	saveProjectInfos();

	//save current scene : 
	if (m_activeScene != nullptr && m_scenes.find(m_activeSceneName) != m_scenes.end())
	{
		m_activeScene->save(activeScenePath);
	}

	//save resources : 
	Json::Value rootResources;
	getMeshFactory().save(rootResources["meshFactory"]);
	getMaterialFactory().save(rootResources["materialFactory"]);
	getTextureFactory().save(rootResources["textureFactory"]);
	getCubeTextureFactory().save(rootResources["cubeTextureFactory"]);

	std::ofstream streamResources;
	streamResources.open(resourcesPath.toString());
	if (!streamResources.is_open())
	{
		std::cout << "error, can't save resources at path : " << resourcesPath.toString() << std::endl;
		return;
	}
	streamResources << rootResources;

	m_scenesStatus[m_activeSceneName] = SceneStatus::EDITED;
}

void Project::load()
{
	//load project infos : 
	FileHandler::CompletePath projectPath(m_projectPath, "projectInfos", ".txt");
	std::ifstream streamProject;
	streamProject.open(projectPath.toString());
	if (!streamProject.is_open())
	{
		std::cout << "error, can't load project infos at path : " << projectPath.toString() << std::endl;
		return;
	}

	//load resources : 
	FileHandler::CompletePath resourcePath(m_projectPath, "resources", ".txt");
	std::ifstream streamResources;
	streamResources.open(resourcePath.toString());
	if (!streamResources.is_open())
	{
		std::cout << "error, can't load project resources at path : " << resourcePath.toString() << std::endl;
		return;
	}
	Json::Value rootResources;
	streamResources >> rootResources;

	getMeshFactory().load(rootResources["meshFactory"]);
	getTextureFactory().load(rootResources["textureFactory"]);
	getCubeTextureFactory().load(rootResources["cubeTextureFactory"]);
	getMaterialFactory().load(rootResources["materialFactory"]);

	//load scenes : 
	Json::Value rootProject;
	streamProject >> rootProject;

	int sceneCount = rootProject.get("sceneCount", 0).asInt();
	for (int i = 0; i < sceneCount; i++)
	{
		std::string sceneName = rootProject["sceneInfos"][i]["name"].asString();
		FileHandler::CompletePath scenePath( m_projectPath.toString() + "scenes/", sceneName, ".txt");
		SceneStatus sceneStatus = (SceneStatus)rootProject["sceneInfos"][i]["status"].asInt();
		m_scenes[sceneName] = scenePath;
		m_scenesStatus[sceneName] = sceneStatus;
	}

	//load the active scene : 
	m_activeSceneName = rootProject.get("activeSceneName", "default").asString();
	if (m_activeScene != nullptr)
		delete m_activeScene;
	m_activeScene = new Scene(m_renderer, m_activeSceneName);
	m_activeScene->load(m_scenes[m_activeSceneName]);

}

void Project::playEdit()
{
	//TODO
}

void Project::play()
{
	//TODO
}

void Project::edit()
{
	//Create the editor : 
	Editor& editor = Editor::instance();

	GLFWwindow* window = Application::get().getActiveWindow();
	Scene* scene = getActiveScene();

	glfwSetDropCallback(window, onFilesDrop);

	float t = 0;

	//main loop
	do
	{
		scene = getActiveScene();

		t = glfwGetTime();
		ImGui_ImplGlfwGL3_NewFrame();

		//get main camera : 
		BaseCamera& currentCamera = editor.getCamera();
		scene->computeCulling();
		//add culling for editor camera if we use it
		if(!editor.getIsPlaying())
			scene->computeCullingForSingleCamera(currentCamera);

		//Physics : 
		scene->updatePhysic(Application::get().getFixedDeltaTime(), currentCamera, editor.getIsPlaying());

		//check if window has been resized by user
		if (Application::get().getWindowResize())
		{
			//TODO : resize only if full screen
			//if (editor.getIsPlaying())
			//	scene->getRenderer().onResizeViewport( glm::vec2(Application::get().getWindowWidth(), Application::get().getWindowHeight()) );

			editor.onResizeWindow();

			Application::get().setWindowResize(false);
		}

		//rendering :
		// TODO : Symplify this
		///////////////////////////////////////////////
		//BEGIN RENDERING THE SCENE
		//renderer.render(camera, entities);
		if (editor.getIsPlaying())
		{
			scene->render(currentCamera);
		}
		else
		{
			scene->renderForEditor(editor.getCamera(), editor.getDebugDrawRenderer());
			CHECK_GL_ERROR("gl error in renderer.");
			//scene->renderPaths(currentCamera);
			//CHECK_GL_ERROR("");
			//scene->renderColliders(currentCamera);
			//CHECK_GL_ERROR("");
			//scene->renderDebugLights(currentCamera);
			//CHECK_GL_ERROR("");
			//scene->renderDebugOctrees(currentCamera);
			//CHECK_GL_ERROR("");
			//scene->renderDebugPhysic(currentCamera);
			//CHECK_GL_ERROR("");

			DebugDrawer::render(currentCamera);
			DebugDrawer::clear();

			editor.renderGizmo(currentCamera);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // TODO : movethis ?
		//END RENDERING THE SCENE
		///////////////////////////////////////////////

		//Update behaviours :
		if (editor.getIsPlaying())
		{
			scene->updateControllers(Application::get().getFixedDeltaTime());
			scene->updateAnimations(Application::get().getTime());
			scene->updateBehaviours();
		}


		//update editor : 
		editor.update(*scene, window);

		//synchronize input handler : 
		InputHandler::synchronize();

#if 1
		/*
		ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("aogl");
		ImGui::SliderFloat("Material Specular Power", &(brickMaterial.specularPower), 0.0f, 100.f);
		lightManager.drawUI();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		*/

		///////////////////////////////////////////////
		//BEGIN RENDERING THE HUD
		ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
		editor.renderUI(*this);
		//END RENDERING THE HUD
		///////////////////////////////////////////////

		//TODO : bouger ça là où il faut
		DragAndDropManager::updateDragAndDrop();

		ImGui::Render();


		glDisable(GL_BLEND);
#endif


		// Check for errors
		checkError("End loop");

		glfwSwapBuffers(window);
		glfwPollEvents();

		double newTime = glfwGetTime();
		Application::get().setDeltaTime( newTime - t );
		Application::get().setFPS( 1.f / (newTime - t) );

	} // Check if the ESC key was pressed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);

}

void Project::exitApplication()
{
	// Close OpenGL window and terminate GLFW
	ImGui_ImplGlfwGL3_Shutdown();
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

Scene* Project::getActiveScene() const
{
	return m_activeScene;
}

Project::SceneStatus Project::getActiveSceneStatus() const
{
	auto findIt = m_scenesStatus.find(m_activeSceneName);
	if (findIt == m_scenesStatus.end())
		return Project::SceneStatus::TEMPORARY;
	else
		return findIt->second;
}

void Project::loadScene(const std::string& sceneName)
{
	if (m_scenes.find(sceneName) == m_scenes.end())
		return;

	if (m_activeScene != nullptr)
	{
		if (m_scenes.find(m_activeSceneName) != m_scenes.end())
		{
			FileHandler::CompletePath activeScenePath = m_scenes[m_activeSceneName];// m_path + "/scenes/" + m_activeSceneName + ".txt";
			addDirectories( FileHandler::Path(m_projectPath.toString() + "scenes/"));
			m_activeScene->save(activeScenePath);
		}

		//m_activeScene->clear();
		delete m_activeScene;
	}

	m_activeScene = new Scene(m_renderer, sceneName);
	if (m_scenesStatus[sceneName] == SceneStatus::DEFAULT) {
		loadDefaultScene(m_activeScene);
	}
	else {
		m_activeScene->load(m_scenes[sceneName]);
	}
	m_activeSceneName = sceneName;

	saveProjectInfos();
}

void Project::addDefaultScene(const std::string& sceneName)
{
	if (m_scenes.find(sceneName) != m_scenes.end())
		return;

	FileHandler::CompletePath scenePath(FileHandler::Path(m_projectPath.toString() + "scenes/"), sceneName, ".txt");
	m_scenes[sceneName] = scenePath;
	m_scenesStatus[sceneName] = SceneStatus::DEFAULT;

	saveProjectInfos();
}

void Project::addSceneFromActive(const std::string& sceneName)
{
	if (m_scenes.find(sceneName) != m_scenes.end())
		return;

	FileHandler::CompletePath scenePath(FileHandler::Path(m_projectPath.toString() + "scenes/"), sceneName, ".txt");
	m_scenes[sceneName] = scenePath;
	m_scenesStatus[sceneName] = SceneStatus::EDITED;

	if (m_activeScene != nullptr)
		m_activeScene->save(scenePath);

	saveProjectInfos();


	//if (m_activeScene != nullptr)
	//{
	//	std::string activeScenePath = m_scenes[m_activeSceneName]; // m_path + "/scenes/" + m_activeSceneName + ".txt";
	//	addDirectories(m_path + "/scenes/");
	//	m_activeScene->save(activeScenePath);
	//	
	//	m_activeScene->clear();
	//	//delete m_activeScene;
	//}

	////m_activeScene = new Scene(m_renderer, sceneName);
	//loadDefaultScene(m_activeScene);
	//m_activeSceneName = sceneName;
}

void Project::saveActiveScene()
{
	if (m_activeScene != nullptr && m_scenes.find(m_activeSceneName) != m_scenes.end()) {
		m_activeScene->save(m_scenes[m_activeSceneName]);
		m_scenesStatus[m_activeSceneName] = SceneStatus::EDITED;
	}
	else {
		std::cout << "can't save the scene, besause the active scene is a temporary scene." << std::endl;
	}

	saveProjectInfos();
}

void Project::reloadActiveScene()
{
	if (m_activeScene == nullptr)
		return;

	delete m_activeScene;
	m_activeScene = new Scene(m_renderer, m_activeSceneName);

	if (m_activeScene != nullptr) {
		m_activeScene->load(m_scenes[m_activeSceneName]);
	}
}


void Project::loadDefaultScene(Scene* scene)
{

	// populate the scene :

	// mesh renderer for colliders : 
	MeshRenderer cubeWireFrameRenderer;
	cubeWireFrameRenderer.setMesh( getMeshFactory().getDefault("cubeWireframe") );
	cubeWireFrameRenderer.setMaterial( getMaterialFactory().getDefault("wireframe"), 0);

	//int r = 5;
	//float omega = 0;
	//for (int i = 0; i < 100; i++)
	//{
	//	Entity* newEntity = new Entity(&scene);
	//	BoxCollider* boxColliderLight = new BoxCollider(&cubeWireFrameRenderer);
	//	PointLight* pointLight = new PointLight(10, glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f), glm::vec3(0,0,0));
	//	newEntity->add(boxColliderLight).add(pointLight);
	//	newEntity->setTranslation(glm::vec3(r*std::cosf(omega), 2.f, r*std::sinf(omega)));
	//	
	//	scene.add(newEntity);

	//	omega += 0.4f;

	//	if(i % 10 == 0)
	//		r += 5;
	//}

	// an entity with a light : 
	Entity* newEntity = new Entity(scene);
	BoxCollider* boxColliderLight = new BoxCollider(getMeshFactory().getDefault("cubeWireframe"), getMaterialFactory().getDefault("wireframe"));
	//SpotLight* spotLight = new SpotLight(10, glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f), glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
	PointLight* pointLight = new PointLight(10, glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f), glm::vec3(0, 0, 0));
	pointLight->setBoundingBoxVisual(getMeshFactory().getDefault("cubeWireframe"), getMaterialFactory().getDefault("wireframe"));
	newEntity->add(boxColliderLight).add(pointLight);
	newEntity->setTranslation(glm::vec3(0, 1.5, 0));
	newEntity->setName("point light");


	//renderers : 
	MeshRenderer* cubeRenderer01 = new MeshRenderer(getMeshFactory().getDefault("cube"), getMaterialFactory().getDefault("brick"));
	MeshRenderer* cubeRenderer02 = new MeshRenderer(getMeshFactory().getDefault("cube"), getMaterialFactory().getDefault("brick"));

	//MeshRenderer cubeRenderer02;
	//cubeRenderer02.mesh = &cube;
	//cubeRenderer02.material = &brickMaterial;

	MeshRenderer* planeRenderer = new MeshRenderer(getMeshFactory().getDefault("plane"), getMaterialFactory().getDefault("brick"));

	//colliders : 
	BoxCollider* boxCollider01 = new BoxCollider(getMeshFactory().getDefault("cubeWireframe"), getMaterialFactory().getDefault("wireframe"));
	BoxCollider* boxCollider02 = new BoxCollider(getMeshFactory().getDefault("cubeWireframe"), getMaterialFactory().getDefault("wireframe"));

	//entities : 

	//cube entity 01
	Entity* entity_cube01 = new Entity(scene);
	entity_cube01->add(cubeRenderer01);
	entity_cube01->add(boxCollider01);
	entity_cube01->setName("cube");
	entity_cube01->setTranslation(glm::vec3(3, 0, 0));
	//cube entity 02
	Entity* entity_cube02 = new Entity(scene);
	entity_cube02->add(cubeRenderer02);
	entity_cube02->add(boxCollider02);
	entity_cube02->setName("cube");
	entity_cube02->setTranslation(glm::vec3(3, -2, 0));
	entity_cube02->setScale(glm::vec3(10, 1, 10));
	/*
	Entity* entity_cube03= new Entity(&scene);
	entity_cube03->add(cubeRenderer03);
	entity_cube03->add(boxCollider03);
	entity_cube03->setScale(glm::vec3(10, 1, 10));
	entity_cube03->setTranslation(glm::vec3(4, 2, 0));
	entity_cube03->setRotation( glm::quat( glm::vec3(0, 0, -glm::pi<float>()*0.5f) ));
	//cube entity 04
	Entity* entity_cube04 = new Entity(&scene);
	entity_cube04->add(cubeRenderer04);
	entity_cube04->add(boxCollider04);
	entity_cube04->setTranslation(glm::vec3(0, -2, 0));
	entity_cube04->setScale(glm::vec3(10, 1, 10));
	entity_cube04->setTranslation(glm::vec3(0, 2, 4));
	entity_cube04->setRotation(glm::quat(glm::vec3(-glm::pi<float>()*0.5f, 0, 0)));
	*/

	//flage entity : 
	Physic::Flag* flag = new Physic::Flag(getMaterialFactory().getDefault("brick"));

	Entity* entity_flag = new Entity(scene);
	entity_flag->add(new BoxCollider(getMeshFactory().getDefault("cubeWireframe"), getMaterialFactory().getDefault("wireframe")));
	entity_flag->add(flag);
	entity_flag->setName("flag");
	entity_flag->endCreation();
}

void Project::setName(const std::string & name)
{
	assert(false);
	//TODO
}

std::string Project::getName()
{
	return m_projectPath[m_projectPath.size() - 1];
}

void Project::setPath(const FileHandler::Path& path)
{
	assert(false);
	//TODO
	//m_projectPath = path;
}

const FileHandler::Path& Project::getPath()
{
	return m_projectPath;
}

const FileHandler::Path& Project::getAssetsFolderPath()
{
	return m_assetFolderPath;
}

const FileHandler::Path& Project::getShaderFolderPath()
{
	return m_shaderFolderPath;
}

FileHandler::CompletePath Project::getAbsolutePathFromRelativePath(const FileHandler::CompletePath& relativeCompletePath)
{
	return FileHandler::CompletePath(getPath().toString() + relativeCompletePath.toString());
}

FileHandler::Path Project::getAbsolutePathFromRelativePath(const FileHandler::Path& relativePath)
{
	return FileHandler::Path(getPath().toString() + relativePath.toString());
}

bool Project::isPathPointingInsideProjectFolder(const FileHandler::Path& path)
{
	return path.toString().find(getPath().toString()) != std::string::npos;
}

bool Project::isPathPointingInsideProjectFolder(const FileHandler::CompletePath& completePath)
{
	return completePath.toString().find(getPath().toString()) != std::string::npos;
}

void Project::drawUI()
{
	if (m_scenes.find(m_activeSceneName) == m_scenes.end()) {
		ImGui::Text("Temporary scene, please add a new scene to the project if you want to save your scene.");
	}
	else {
		ImGui::Text(("active scene : " + m_activeSceneName).c_str());
		ImGui::SameLine();
		if (ImGui::Button("save")) {
			saveActiveScene();
		}
	}


	ImGui::InputText("new scene", m_newSceneName, 30);
	ImGui::SameLine();
	if (ImGui::Button("add blank")) {
		addDefaultScene(m_newSceneName);
	}
	ImGui::SameLine();
	if(ImGui::Button("add from active")) {
		addSceneFromActive(m_newSceneName);
	}

	for (auto& it = m_scenes.begin(); it != m_scenes.end(); it++)
	{
		ImGui::PushID(it->first.c_str());
		ImGui::Text(it->first.c_str());
		ImGui::SameLine();
		if (ImGui::Button("load"))
			loadScene(it->first);
		ImGui::PopID();
	}
}

GLFWwindow* Project::initGLFW(int width, int height)
{

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
	glfwWindowHint(GLFW_DECORATED, GL_TRUE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

#if defined(__APPLE__)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	int const DPI = 2; // For retina screens only
#else
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	int const DPI = 1;
# endif

	// Open a window and create its OpenGL context
	GLFWwindow* window = glfwCreateWindow(width / DPI, height / DPI, "aogl", 0, 0);
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	return window;
}

void Project::initGlew()
{
	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}
}

void Project::initImGUI()
{
	//TODO
}

void Project::setupWindow(GLFWwindow* window)
{
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Enable vertical sync (on cards that support it)
	glfwSwapInterval(1);
	GLenum glerr = GL_NO_ERROR;
	glerr = glGetError();

	ImGui_ImplGlfwGL3_Init(window, true);

	//set the resize window callback 
	glfwSetWindowSizeCallback(window, onWindowResize);
}

void Project::initDefaultAssets()
{
	////////////////////// SKYBOX shaders ////////////////////////
	//// Try to load and compile shaders
	//GLuint vertShaderId_skybox = compile_shader_from_file(GL_VERTEX_SHADER, "skybox.vert");
	//GLuint fragShaderId_skybox = compile_shader_from_file(GL_FRAGMENT_SHADER, "skybox.frag");

	//GLuint programObject_skybox = glCreateProgram();
	//glAttachShader(programObject_skybox, vertShaderId_skybox);
	//glAttachShader(programObject_skybox, fragShaderId_skybox);

	//glLinkProgram(programObject_skybox);
	//if (check_link_error(programObject_skybox) < 0)
	//	exit(1);

	////check uniform errors : 
	//if (!checkError("Uniforms"))
	//	exit(1);

	////////////////////// 3D Gpass shaders ////////////////////////
	//// Try to load and compile shaders
	//GLuint vertShaderId_gpass = compile_shader_from_file(GL_VERTEX_SHADER, "aogl.vert");
	//GLuint fragShaderId_gpass = compile_shader_from_file(GL_FRAGMENT_SHADER, "aogl_gPass.frag");

	//GLuint programObject_gPass = glCreateProgram();
	//glAttachShader(programObject_gPass, vertShaderId_gpass);
	//glAttachShader(programObject_gPass, fragShaderId_gpass);

	//glLinkProgram(programObject_gPass);
	//if (check_link_error(programObject_gPass) < 0)
	//	exit(1);

	////check uniform errors : 
	//if (!checkError("Uniforms"))
	//	exit(1);

	////////////////////// WIREFRAME shaders ////////////////////////
	//// Try to load and compile shaders
	//GLuint vertShaderId_wireframe = compile_shader_from_file(GL_VERTEX_SHADER, "wireframe.vert");
	//GLuint fragShaderId_wireframe = compile_shader_from_file(GL_FRAGMENT_SHADER, "wireframe.frag");

	//GLuint programObject_wireframe = glCreateProgram();
	//glAttachShader(programObject_wireframe, vertShaderId_wireframe);
	//glAttachShader(programObject_wireframe, fragShaderId_wireframe);

	//glLinkProgram(programObject_wireframe);
	//if (check_link_error(programObject_wireframe) < 0)
	//	exit(1);

	////check uniform errors : 
	//if (!checkError("Uniforms"))
	//	exit(1);

	////////////////////// TERRAIN shaders ////////////////////////
	//// Try to load and compile shaders
	//GLuint vertShaderId_terrain = compile_shader_from_file(GL_VERTEX_SHADER, "terrain.vert");
	//GLuint fragShaderId_terrain = compile_shader_from_file(GL_FRAGMENT_SHADER, "terrain.frag");

	//GLuint programObject_terrain = glCreateProgram();
	//glAttachShader(programObject_terrain, vertShaderId_terrain);
	//glAttachShader(programObject_terrain, fragShaderId_terrain);

	//glLinkProgram(programObject_terrain);
	//if (check_link_error(programObject_terrain) < 0)
	//	exit(1);

	////check uniform errors : 
	//if (!checkError("Uniforms"))
	//	exit(1);

	////////////////////// TERRAIN EDITION shaders ////////////////////////
	//// Try to load and compile shaders
	//GLuint vertShaderId_terrainEdition = compile_shader_from_file(GL_VERTEX_SHADER, "terrainEdition.vert");
	//GLuint fragShaderId_terrainEdition = compile_shader_from_file(GL_FRAGMENT_SHADER, "terrainEdition.frag");

	//GLuint programObject_terrainEdition = glCreateProgram();
	//glAttachShader(programObject_terrainEdition, vertShaderId_terrainEdition);
	//glAttachShader(programObject_terrainEdition, fragShaderId_terrainEdition);

	//glLinkProgram(programObject_terrainEdition);
	//if (check_link_error(programObject_terrainEdition) < 0)
	//	exit(1);

	////check uniform errors : 
	//if (!checkError("Uniforms"))
	//	exit(1);

	////////////////////// DRAW ON TEXTURE shaders ////////////////////////
	//// Try to load and compile shaders
	//GLuint vertShaderId_drawOnTexture = compile_shader_from_file(GL_VERTEX_SHADER, "drawOnTexture.vert");
	//GLuint fragShaderId_drawOnTexture = compile_shader_from_file(GL_FRAGMENT_SHADER, "drawOnTexture.frag");

	//GLuint programObject_drawOnTexture = glCreateProgram();
	//glAttachShader(programObject_drawOnTexture, vertShaderId_drawOnTexture);
	//glAttachShader(programObject_drawOnTexture, fragShaderId_drawOnTexture);

	//glLinkProgram(programObject_drawOnTexture);
	//if (check_link_error(programObject_drawOnTexture) < 0)
	//	exit(1);

	////check uniform errors : 
	//if (!checkError("Uniforms"))
	//	exit(1);

	////////////////////// GRASS FIELD shaders ////////////////////////
	//// Try to load and compile shaders
	//GLuint vertShaderId_grassField = compile_shader_from_file(GL_VERTEX_SHADER, "grassField.vert");
	//GLuint fragShaderId_grassField = compile_shader_from_file(GL_FRAGMENT_SHADER, "grassField.frag");

	//GLuint programObject_grassField = glCreateProgram();
	//glAttachShader(programObject_grassField, vertShaderId_grassField);
	//glAttachShader(programObject_grassField, fragShaderId_grassField);

	//glLinkProgram(programObject_grassField);
	//if (check_link_error(programObject_grassField) < 0)
	//	exit(1);

	////check uniform errors : 
	//if (!checkError("Uniforms"))
	//	exit(1);


	// cube and plane ;

	//Mesh cube;
	//cube.triangleIndex = { 0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
	//cube.uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f, };
	//cube.vertices = { -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 
	//				-0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5,
	//				-0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5,
	//				-0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 
	//				0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 
	//				0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5,
	//				-0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
	//cube.normals = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
	//				0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
	//				0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1,
	//				0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0,
	//				1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
	//				1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0,
	//				-1, 0, 0, -1, 0, 0, -1, 0, 0, };
	//cube.initGl();

	/*Mesh* cube = new Mesh(GL_TRIANGLES, (Mesh::USE_INDEX | Mesh::USE_VERTICES | Mesh::USE_NORMALS | Mesh::USE_UVS | Mesh::USE_TANGENTS));
	cube->vertices = { 0.5,0.5,-0.5,  0.5,0.5,0.5,  0.5,-0.5,0.5,  0.5,-0.5,-0.5,
		-0.5,0.5,-0.5,  -0.5,0.5,0.5,  -0.5,-0.5,0.5,  -0.5,-0.5,-0.5,
		-0.5,0.5,0.5,  0.5,0.5,0.5,  0.5,-0.5,0.5,  -0.5,-0.5,0.5,
		-0.5,0.5,-0.5,  0.5,0.5,-0.5,  0.5,-0.5,-0.5,  -0.5,-0.5,-0.5,
		0.5,0.5,0.5, -0.5,0.5,0.5, -0.5,0.5,-0.5, 0.5,0.5,-0.5,
		-0.5,-0.5,-0.5,  0.5,-0.5,-0.5,  0.5,-0.5,0.5,  -0.5,-0.5,0.5 };

	cube->normals = { 1,0,0,  1,0,0,  1,0,0,  1,0,0,
		-1,0,0,  -1,0,0,  -1,0,0,  -1,0,0,
		0,0,1,  0,0,1,  0,0,1,  0,0,1,
		0,0,-1,  0,0,-1,  0,0,-1,  0,0,-1,
		0,1,0,  0,1,0,  0,1,0,  0,1,0,
		0,-1,0,  0,-1,0,  0,-1,0,  0,-1,0 };

	cube->tangents = { 0,0,1,  0,0,1,  0,0,1,  0,0,1,
		0,0,1,  0,0,1,  0,0,1,  0,0,1,
		-1,0,0,  -1,0,0,  -1,0,0,  -1,0,0,
		1,0,0,  1,0,0,  1,0,0,  1,0,0,
		1,0,0,  1,0,0,  1,0,0,  1,0,0,
		-1,0,0,  -1,0,0,  -1,0,0,  -1,0,0 };

	cube->uvs = { 0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0 };

	cube->triangleIndex = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20 };

	cube->initGl();


	Mesh* cubeWireFrame = new Mesh(GL_LINE_STRIP, (Mesh::USE_INDEX | Mesh::USE_VERTICES));
	cubeWireFrame->triangleIndex = { 0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
	cubeWireFrame->uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f, };
	cubeWireFrame->vertices = { -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
	cubeWireFrame->normals = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
	cubeWireFrame->initGl();

	Mesh* plane = new Mesh();
	plane->triangleIndex = { 0, 1, 2, 2, 1, 3 };
	plane->uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f };
	plane->vertices = { -5.0, -0.5, 5.0, 5.0, -0.5, 5.0, -5.0, -0.5, -5.0, 5.0, -0.5, -5.0 };
	plane->normals = { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 };
	plane->initGl();*/

	/*
	int x;
	int y;
	int comp;

	unsigned char * diffuse = stbi_load("textures/spnza_bricks_a_diff.tga", &x, &y, &comp, 3);
	GLuint diffuseTexture;
	glGenTextures(1, &diffuseTexture);

	glBindTexture(GL_TEXTURE_2D, diffuseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuse);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	unsigned char * specular = stbi_load("textures/spnza_bricks_a_spec.tga", &x, &y, &comp, 3);
	GLuint specularTexture;
	glGenTextures(1, &specularTexture);

	glBindTexture(GL_TEXTURE_2D, specularTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, specular);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	*/


	//Important : We init all default resources
	initAllResourceFactories();


	Texture* diffuseTexture = new Texture(FileHandler::CompletePath("textures/spnza_bricks_a_diff.tga"));
	Texture* specularTexture = new Texture(FileHandler::CompletePath("textures/spnza_bricks_a_spec.tga"));
	Texture* bumpTexture = new Texture(FileHandler::CompletePath("textures/spnza_bricks_a_normal.png"));

	Texture* grassTextureDiffuse = new Texture(FileHandler::CompletePath("textures/grass/grass01.png"), true);
	grassTextureDiffuse->textureWrapping_u = GL_CLAMP_TO_EDGE;
	grassTextureDiffuse->textureWrapping_v = GL_CLAMP_TO_EDGE;

	Texture* texSkyRight = new Texture(FileHandler::CompletePath("textures/skyboxes/right.png"));
	Texture* texSkyLeft = new Texture(FileHandler::CompletePath("textures/skyboxes/left.png"));
	Texture* texSkyTop = new Texture(FileHandler::CompletePath("textures/skyboxes/top.png"));
	Texture* texSkyBottom = new Texture(FileHandler::CompletePath("textures/skyboxes/bottom.png"));
	Texture* texSkyFront = new Texture(FileHandler::CompletePath("textures/skyboxes/front.png"));
	Texture* texSkyBack = new Texture(FileHandler::CompletePath("textures/skyboxes/back.png"));

	getTextureFactory().addDefault("skyRight", texSkyRight);
	getTextureFactory().addDefault("skyLeft", texSkyLeft);
	getTextureFactory().addDefault("skyTop", texSkyTop);
	getTextureFactory().addDefault("skyBottom", texSkyBottom);
	getTextureFactory().addDefault("skyFront", texSkyFront);
	getTextureFactory().addDefault("skyBack", texSkyBack);

	//std::vector<FileHandler::CompletePath> skyboxTexturePaths = { 
	//	FileHandler::CompletePath("textures/skyboxes/right.png"), FileHandler::CompletePath("textures/skyboxes/left.png"),
	//	FileHandler::CompletePath("textures/skyboxes/top.png"), FileHandler::CompletePath("textures/skyboxes/top.png"),
	//	FileHandler::CompletePath("textures/skyboxes/front.png"),FileHandler::CompletePath("textures/skyboxes/back.png") 
	//};
	std::vector<ResourcePtr<Texture>> skyboxTextureBundle = {
		getTextureFactory().getDefault("skyRight"),
		getTextureFactory().getDefault("skyLeft"),
		getTextureFactory().getDefault("skyTop"),
		getTextureFactory().getDefault("skyBottom"),
		getTextureFactory().getDefault("skyFront"),
		getTextureFactory().getDefault("skyBack")
	};

	CubeTexture* defaultSkybox = new CubeTexture(skyboxTextureBundle);

	//force texture initialisation
	diffuseTexture->initGL();
	specularTexture->initGL();
	bumpTexture->initGL();
	grassTextureDiffuse->initGL();

	//////////////////// BEGIN RESSOURCES : 
	//the order between resource initialization and factories initialisation is important, indeed it's the factory which set set name of the different ressources when they are added to the factories.
	// So initialyzing materials before TextureFectory initialysation will create materials with wrong texture and mesh names. 

	////////// INITIALYZE DEFAULT TEXTURES IN FACTORY : 
	getTextureFactory().addDefault("brickDiffuse", diffuseTexture);
	getTextureFactory().addDefault("brickSpecular", specularTexture);
	getTextureFactory().addDefault("brickBump", bumpTexture);
	getTextureFactory().addDefault("grass01Diffuse", grassTextureDiffuse);

	// materials : 
	//MaterialLit* defaultMaterial = new MaterialLit(programObject_gPass, getTextureFactory().get("default"), getTextureFactory().get("default"), getTextureFactory().get("default"), 50);
	MaterialLit* brickMaterial = new MaterialLit(*getProgramFactory().get("lit"));//new MaterialLit(getProgramFactory().getDefault("defaultLit")->id /*programObject_gPass*/, diffuseTexture, specularTexture, bumpTexture, 50);
	brickMaterial->setInternalData<ResourcePtr<Texture>>("Diffuse", &getTextureFactory().getDefault("brickDiffuse"));
	brickMaterial->setInternalData<ResourcePtr<Texture>>("Bump", &getTextureFactory().getDefault("brickBump"));
	brickMaterial->setInternalData<ResourcePtr<Texture>>("Specular", &getTextureFactory().getDefault("brickSpecular"));
																				  //MaterialUnlit* wireframeMaterial = new MaterialUnlit(programObject_wireframe);
	//MaterialGrassField* grassFieldMaterial = new MaterialGrassField(programObject_grassField);

	////////// INITIALYZE DEFAULT MATERIALS IN FACTORY : 
	//getMaterialFactory().add("default", defaultMaterial);
	getMaterialFactory().addDefault("brick", brickMaterial);
	//getMaterialFactory().add("wireframe", wireframeMaterial);
	//getMaterialFactory().add("grassField", grassFieldMaterial);

	////////// INITIALYZE DEFAULT MESHES IN FACTORY : 
	//getMeshFactory().add("cube", cube);
	//getMeshFactory().add("cubeWireframe", cubeWireFrame);
	//getMeshFactory().add("plane", plane);

	getCubeTextureFactory().addDefault("plaineSkybox", defaultSkybox);

	////////// INITIALYZE DEFAULT PROGRAMS IN FACTORY : 
	//getProgramFactory().add("defaultLit", programObject_gPass);
	//getProgramFactory().add("defaultUnlit", programObject_wireframe);
	//getProgramFactory().add("defaultSkybox", programObject_skybox);
	//getProgramFactory().add("defaultTerrain", programObject_terrain);
	//getProgramFactory().add("defaultTerrainEdition", programObject_terrainEdition);
	//getProgramFactory().add("defaultDrawOnTexture", programObject_drawOnTexture);
	//getProgramFactory().add("defaultGrassField", programObject_grassField);

	///////////////////// END RESSOURCES 

}

void Project::initProject()
{
	//we need set the projectPath before to call this function.
	assert(m_projectPath.toString() != "");

	initDefaultAssets();

	// create and initialize our light manager
	LightManager* lightManager = new LightManager();

	// renderer : 
	m_renderer = new Renderer(lightManager, "aogl.vert", "aogl_gPass.frag", "aogl_lightPass.vert", "aogl_lightPass_pointLight.frag", "aogl_lightPass_directionalLight.frag", "aogl_lightPass_spotLight.frag"); // call lightManager.init()
	m_renderer->initPostProcessQuad("blit.vert", "blit.frag");
	m_renderer->initialyzeShadowMapping("shadowPass.vert", "shadowPass.frag", "shadowPassOmni.vert", "shadowPassOmni.frag", "shadowPassOmni.geom");
}
