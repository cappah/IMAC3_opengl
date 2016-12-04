#pragma once


#include <memory>
#include "EditorNodes.h"
#include "ResourceTree.h"

//forwards
class Editor;
class EditorModal;
class Inspector;
class SceneHierarchy;
class DebugDrawRenderer;
class Viewport;

class DroppedFileEditorFrame : public EditorFrame
{
private:
	ResourceTreeView m_treeView;
	//static DroppedFileEditorWindow* modalRef;
	Editor* m_editorRef;

public:
	DroppedFileEditorFrame(Editor* editorRef);

	//static void openPopUp(Editor& editor);
	//static void closePopUp(Editor& editor);
	void drawContent(Project& project, EditorModal* parentWindow) override;
};

/////////////////////////////////////////

class ViewportEditorFrame : public EditorFrame
{
private:
	std::weak_ptr<Viewport> m_viewport;

public:
	ViewportEditorFrame(const std::string& name, std::shared_ptr<Viewport> model);
	void drawContent(Project& project, EditorModal* parentWindow) override;
	void onFrameMoved() override;
	void onFrameResized() override;
};

/////////////////////////////////////////

class TerrainToolEditorFrame : public EditorFrame
{
public:
	TerrainToolEditorFrame(const std::string& name);
	void drawContent(Project& project, EditorModal* parentWindow) override;
};

/////////////////////////////////////////

class SkyboxToolEditorFrame : public EditorFrame
{
public:
	SkyboxToolEditorFrame(const std::string& name);
	void drawContent(Project& project, EditorModal* parentWindow) override;
};

/////////////////////////////////////////

class SceneManagerEditorFrame : public EditorFrame
{
public:
	SceneManagerEditorFrame(const std::string& name);
	void drawContent(Project& project, EditorModal* parentWindow) override;
};

/////////////////////////////////////////

class FactoriesDebugEditorFrame : public EditorFrame
{
private:
	bool m_textureFactoryVisible;
	bool m_cubeTextureFactoryVisible;
	bool m_meshFactoryVisible;
	bool m_programFactoryVisible;
	bool m_materialFactoryVisible;
	bool m_skeletalAnimationFactoryVisible;

public:
	FactoriesDebugEditorFrame(const std::string& name);
	void hideAllDebugViews();
	void drawContent(Project& project, EditorModal* parentWindow) override;
};

/////////////////////////////////////////

class InspectorEditorFrame : public EditorFrame
{
private:
	std::weak_ptr<Inspector> m_inspector;

public:
	InspectorEditorFrame(const std::string& name, std::shared_ptr<Inspector> model);
	void drawContent(Project& project, EditorModal* parentWindow) override;
};

/////////////////////////////////////////

class DebugRenderEditorFrame : public EditorFrame
{
private:
	std::weak_ptr<DebugDrawRenderer> m_debugDrawRenderer;
public:
	DebugRenderEditorFrame(const std::string& name, std::shared_ptr<DebugDrawRenderer> model);
	void drawContent(Project& project, EditorModal* parentWindow) override;
};

/////////////////////////////////////////

class SceneHierarchyEditorFrame : public EditorFrame
{
private:
	std::weak_ptr<SceneHierarchy> m_sceneHierarchy;

public:
	SceneHierarchyEditorFrame(const std::string& name, std::shared_ptr<SceneHierarchy> model);
	void drawContent(Project& project, EditorModal* parentWindow) override;
};

/////////////////////////////////////////
