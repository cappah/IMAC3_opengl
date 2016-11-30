#pragma once


#include <memory>
#include "EditorNodes.h"
#include "ResourceTree.h"

//forwards
class Editor;
class EditorModal;

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

public:
	ViewportEditorFrame(const std::string& name);
	void drawContent(Project& project, EditorModal* parentWindow) override;
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