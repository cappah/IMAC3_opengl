#pragma once

#include "glew/glew.h"

#include "Application.h"
#include "InputHandler.h"
#include "Factories.h"
#include "Scene.h"
#include "Editor.h"

class Project
{
private:
	std::vector<Scene*> m_scenes;
	int m_activeSceneIdx;

	Renderer* m_renderer;

	//meta : 
	std::string m_name;
	std::string m_path;

public:
	Project();
	~Project();

	//create and init default assets for the project.
	void init();
	//deallocate project memory.
	void clear();
	void open(const std::string& projectName, const std::string& projectPath);

	//save all scenes and resources.
	void save();
	//load a project (scenes and resources).
	void load();

	void playEdit();
	void play();
	void edit();
	void exitApplication();

	Scene* getActiveScene() const;
	void loadScene(int sceneIdx);
	void loadScene(const std::string& sceneName);

	void loadDefaultScene(Scene* scene);

	void setName(const std::string& name);
	std::string getName() const;

	void setPath(const std::string& path);
	std::string getPath() const;


private :
	GLFWwindow* Project::initGLFW(int width, int height);
	void initGlew();
	void initImGUI();
	void setupWindow(GLFWwindow* window);
	//void onWindowResize(GLFWwindow* window, int width, int height);
	void initDefaultAssets();
};

