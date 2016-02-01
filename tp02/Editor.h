#pragma once

#include "Entity.h"
#include "Gizmo.h"
#include "InputHandler.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "GLFW/glfw3.h"

//forward
class Scene;


class Inspector
{
private:
	char textValue[30];
	int intValue;
	float floatValue;
	glm::vec3 vector3Value;

public:
	Inspector();
	~Inspector();

	void drawUI(const std::vector<PointLight*>& pointLights);
	void drawUI(const std::vector<DirectionalLight*>& directionalLights);
	void drawUI(const std::vector<SpotLight*>& spotLights);
	void drawUI(const std::vector<Entity*>& entities);
	void drawUI(const std::vector<MeshRenderer*>& meshRenderers);
	void drawUI(const std::vector<Collider*>& colliders);
};

/////////////////////////////////////////

struct GUIStates
{
	bool panLock;
	bool turnLock;
	bool zoomLock;
	int lockPositionX;
	int lockPositionY;
	int camera;
	double time;
	bool playing;
	static const float MOUSE_PAN_SPEED;
	static const float MOUSE_ZOOM_SPEED;
	static const float MOUSE_TURN_SPEED;

	int leftButton;
	int rightButton;
	int middleButton;

	int altPressed;
	int shiftPressed;
	int ctrlPressed;

	bool leftPressed;
	bool rightPressed;
	bool forwardPressed;
	bool backwardPressed;
};

void init_gui_states(GUIStates & guiStates);

/////////////////////////////////////////


class Editor
{

private:
	//current entity selected
	std::vector<Entity*> m_currentSelected;
	//current components selected, for multiple editing
	std::vector<PointLight*> m_pointLights;
	std::vector<DirectionalLight*> m_directionlLights;
	std::vector<SpotLight*> m_spotLights;
	std::vector<MeshRenderer*> m_meshRenderers;
	std::vector<Collider*> m_colliders;

	Gizmo* m_gizmo;

	Gizmo::GizmoArrowType m_gizmoTranslationDirection;
	glm::vec3 m_gizmoTranslationAnchor;
	bool m_isMovingGizmo;

	bool m_isUIVisible;
	bool m_isGizmoVisible;

	bool m_terrainToolVisible;
	bool m_skyboxToolVisible;
	bool m_textureFactoryVisible;
	bool m_cubeTextureFactoryVisible;
	bool m_meshFactoryVisible;
	bool m_programFactoryVisible;
	bool m_materialFactoryVisible;

	bool m_multipleEditing;

	Inspector m_inspector;

	GUIStates m_guiStates;

	Camera* m_camera;
	bool m_cameraFPS;

	bool m_hideCursorWhenMovingCamera;

	//window splitter : 
	float m_leftPanelwidth;
	float m_leftPanelHeight;

public:
	Editor(MaterialUnlit* _unlitMaterial);

	void changeCurrentSelected(Entity* entity);
	void changeCurrentSelected(std::vector<Entity*> entities);
	void addCurrentSelected(Entity* entity);
	void removeCurrentSelected(Entity* entity);
	void toggleCurrentSelected(Entity* entity);

	void renderGizmo();

	void renderUI(Scene& scene);

	bool testGizmoIntersection(const Ray& ray);
	void beginMoveGizmo();
	bool isMovingGizmo();
	void endMoveGizmo();
	void moveGizmo(const Ray& ray);

	Entity* duplicateSelected();

	void deleteSelected(Scene& scene);

	void toggleUIVisibility();
	void toggleGizmoVisibility();
	void toggleDebugVisibility(Scene& scene);
	void toggleLightsBoundingBoxVisibility(Scene& scene);

	void update(/*Camera & camera*/ Scene& scene, GLFWwindow* window, InputHandler& inputHandler);

	Camera& getCamera();

	void updateGuiStates(GLFWwindow* window);
	void updateCameraMovement_editor(GLFWwindow* window);
	void updateCameraMovement_fps(GLFWwindow* window);

	//for multiple editing : 
	void clearSelectedComponents();
	void refreshSelectedComponents(bool clearComponentLists = true);

	glm::vec4 getTopLeftWindowViewport() const;
	glm::vec4 getTopRightWindowViewport() const;
	glm::vec4 getBottomWindowViewport() const;

	void onResizeWindow();
};

