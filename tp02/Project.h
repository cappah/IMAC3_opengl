#pragma once

#include "glew/glew.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Application.h"
#include "InputHandler.h"
#include "Factories.h"
#include "Scene.h"
#include "Editor.h"

class Project
{
	enum SceneStatus {DEFAULT, EDITED};

private:
	// mapping scenes name <-> scenes path 
	std::map<std::string, std::string> m_scenes;
	std::map<std::string, SceneStatus> m_scenesStatus;
	std::string m_activeSceneName;
	Scene* m_activeScene;

	Renderer* m_renderer;

	//meta : 
	std::string m_name;
	std::string m_path;

	//For ui : 
	char m_newSceneName[30];

public:
	Project();
	~Project();

	//create and init default assets for the project.
	void init();
	//deallocate project memory.
	void clear();
	void open(const std::string& projectName, const std::string& projectPath);

	void saveProjectInfos();
	//save all scenes and resources.
	void save();
	//load a project (scenes and resources).
	void load();

	void playEdit();
	void play();
	void edit();
	void exitApplication();

	Scene* getActiveScene() const;
	void loadScene(const std::string& sceneName);
	void addDefaultScene(const std::string& sceneName);
	void addSceneFromActive(const std::string& sceneName);
	//simply save the active scene : 
	void saveActiveScene();
	//reload the active scene, without saving it : 
	void reloadActiveScene();

	void loadDefaultScene(Scene* scene);

	void setName(const std::string& name);
	std::string getName() const;

	void setPath(const std::string& path);
	std::string getPath() const;

	void drawUI();


private :
	GLFWwindow* Project::initGLFW(int width, int height);
	void initGlew();
	void initImGUI();
	void setupWindow(GLFWwindow* window);
	//void onWindowResize(GLFWwindow* window, int width, int height);
	void initDefaultAssets();
	void initProject();
};

