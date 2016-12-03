#pragma once

#include <map>
#include <string>

#include <glew/glew.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include <GLFW/glfw3.h>

#include "FileHandler.h"

//forwards : 
class Renderer;
class Scene;
struct GLFWwindow;

class Project
{
public : enum SceneStatus {DEFAULT, EDITED, TEMPORARY};

private:
	// mapping scenes name <-> scenes path 
	std::map<std::string, FileHandler::CompletePath> m_scenes;
	std::map<std::string, SceneStatus> m_scenesStatus;
	std::string m_activeSceneName;
	Scene* m_activeScene;

	Renderer* m_renderer;

	//meta : 
	static FileHandler::Path m_projectPath;
	static FileHandler::Path m_assetFolderPath;
	static FileHandler::Path m_shaderFolderPath;

	//For ui : 
	char m_newSceneName[30];

public:
	Project();
	~Project();

	//create and init default assets for the project.
	void init();
	//deallocate project memory.
	void clear();
	void open(const std::string& projectName, const FileHandler::Path& projectPath);

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
	Project::SceneStatus getActiveSceneStatus() const;
	void loadScene(const std::string& sceneName);
	void addDefaultScene(const std::string& sceneName);
	void addSceneFromActive(const std::string& sceneName);
	//simply save the active scene : 
	void saveActiveScene();
	//reload the active scene, without saving it : 
	void reloadActiveScene();

	void loadDefaultScene(Scene* scene);

	void drawUI();
	
	//accessing global meta data about project.
	static void setName(const std::string& name);
	static std::string getName();
	static void setPath(const FileHandler::Path& path);
	static const FileHandler::Path& getPath();
	static const FileHandler::Path& getAssetsFolderPath();
	static const FileHandler::Path& getShaderFolderPath();
	static FileHandler::CompletePath getAbsolutePathFromRelativePath(const FileHandler::CompletePath& relativeCompletePath);
	static FileHandler::Path getAbsolutePathFromRelativePath(const FileHandler::Path& relativePath);
	static bool isPathPointingInsideProjectFolder(const FileHandler::Path& path);
	static bool isPathPointingInsideProjectFolder(const FileHandler::CompletePath& completePath);

private :
	GLFWwindow* Project::initGLFW(int width, int height);
	void initGlew();
	void initImGUI();
	void setupWindow(GLFWwindow* window);
	//void onWindowResize(GLFWwindow* window, int width, int height);
	void initDefaultAssets();
	void initProject();
};

