#include "Editor.h"
#include "EditorFrames.h"
#include "Texture.h"
#include "Project.h" //forward
#include "Scene.h" //forward
#include "EditorWindows.h" //forward

//DroppedFileEditorWindow* DroppedFileEditorWindow::modalRef = nullptr;

DroppedFileEditorFrame::DroppedFileEditorFrame(Editor* editorRef)
	: EditorFrame("DroppedFileWindow")
	, m_treeView("DroppedFileWindow")
	, m_editorRef(editorRef)
{
	assert(editorRef != nullptr);
	m_treeView.setModel(m_editorRef->getResourceTree());
}
//
//void DroppedFileEditorWindow::openPopUp(Editor& editor)
//{
//	auto modaleSharedRef = std::make_shared<DroppedFileEditorWindow>(&editor);
//	modalRef = modaleSharedRef.get();
//	modalRef->treeView.setModel(editor.getResourceTree());
//	editor.addModal(modaleSharedRef);
//}
//
//void DroppedFileEditorWindow::closePopUp(Editor& editor)
//{
//	if (!modalRef)
//		return;
//
//	editor.removeModal(modalRef);
//	modalRef = nullptr;
//}

void DroppedFileEditorFrame::drawContent(Project& project, EditorModal* parentWindow)
{
	ImGui::Columns(2);

	//draw file list
	for (int i = 0; i < m_editorRef->getDroppedFilesCount(); i++)
	{
		ImGui::Button(m_editorRef->getDroppedFilesPath(i).c_str());
		//files drag and drop
		if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(0) && ImGui::IsMouseDown(0))
		{
			DragAndDropManager::beginDragAndDrop(std::make_shared<DroppedFileDragAndDropOperation>(m_editorRef->getDroppedFilesPath(i)));
		}
	}

	ImGui::NextColumn();

	//draw resource tree 
	ImGui::BeginChild("resourceTreeView");
	m_treeView.drawContent(project);
	ImGui::EndChild();

	ImGui::Columns(1);

	ImGui::Separator();

	if (ImGui::Button("Done"))
	{
		m_editorRef->clearDroppedFiles();
		parentWindow->closeModal();
	}
}

///////////////////////////////

ViewportEditorFrame::ViewportEditorFrame(const std::string& name)
	: EditorFrame(name)
{
}

void ViewportEditorFrame::drawContent(Project & project, EditorModal * parentWindow)
{
	Texture* finalFrame = project.getActiveScene()->getRenderer().getFinalFrame();
	ImGui::Image((void*)(finalFrame->glId), ImVec2(finalFrame->w, finalFrame->h));
}

///////////////////////////////

TerrainToolEditorFrame::TerrainToolEditorFrame(const std::string& name)
	: EditorFrame(name)
{}

void TerrainToolEditorFrame::drawContent(Project& project, EditorModal* parentWindow)
{
	Scene* scene = project.getActiveScene();
	scene->getTerrain().drawUI();
}

//////////////////////////////

SkyboxToolEditorFrame::SkyboxToolEditorFrame(const std::string& name)
	: EditorFrame(name)
{}

void SkyboxToolEditorFrame::drawContent(Project& project, EditorModal* parentWindow)
{
	Scene* scene = project.getActiveScene();
	scene->getSkybox().drawUI();
}

//////////////////////////////

SceneManagerEditorFrame::SceneManagerEditorFrame(const std::string& name)
	: EditorFrame(name)
{}

void SceneManagerEditorFrame::drawContent(Project& project, EditorModal* parentWindow)
{
	project.drawUI();
}

//////////////////////////////

FactoriesDebugEditorFrame::FactoriesDebugEditorFrame(const std::string& name)
	: EditorFrame(name)
{
	hideAllDebugViews();
	m_textureFactoryVisible = true;
}

void FactoriesDebugEditorFrame::hideAllDebugViews()
{
	m_textureFactoryVisible = false;
	m_cubeTextureFactoryVisible = false;
	m_meshFactoryVisible = false;
	m_programFactoryVisible = false;
	m_materialFactoryVisible = false;
	m_skeletalAnimationFactoryVisible = false;
}

void FactoriesDebugEditorFrame::drawContent(Project& project, EditorModal* parentWindow)
{
	if (ImGui::RadioButton("texture factory", m_textureFactoryVisible))
	{
		hideAllDebugViews();
		m_textureFactoryVisible = true;
	}
	if (ImGui::RadioButton("cube texture factory", m_cubeTextureFactoryVisible))
	{
		hideAllDebugViews();
		m_cubeTextureFactoryVisible = true;
	}
	if (ImGui::RadioButton("mesh factory", m_meshFactoryVisible))
	{
		hideAllDebugViews();
		m_meshFactoryVisible = true;
	}
	if (ImGui::RadioButton("program factory", m_programFactoryVisible))
	{
		hideAllDebugViews();
		m_programFactoryVisible = true;
	}
	if (ImGui::RadioButton("material factory", m_materialFactoryVisible))
	{
		hideAllDebugViews();
		m_materialFactoryVisible = true;
	}
	if (ImGui::RadioButton("skeletal animation factory", m_skeletalAnimationFactoryVisible))
	{
		hideAllDebugViews();
		m_skeletalAnimationFactoryVisible = true;
	}

	//////////////////////

	//TODO : implement drawUI in factories
	/*
	if (m_textureFactoryVisible)
	{
	ImGui::BeginChild("Texture factory");
	getTextureFactory().drawUI();
	ImGui::End();
	}

	if (m_cubeTextureFactoryVisible)
	{
	ImGui::BeginChild("Cube Texture factory");
	getCubeTextureFactory().drawUI();
	ImGui::End();
	}

	if (m_meshFactoryVisible)
	{
	ImGui::BeginChild("Mesh factory");
	getMeshFactory().drawUI();
	ImGui::End();
	}

	if (m_programFactoryVisible)
	{
	ImGui::BeginChild("Program factory");
	getProgramFactory().drawUI();
	ImGui::End();
	}

	if (m_materialFactoryVisible)
	{
	ImGui::BeginChild("Material factory");
	getMaterialFactory().drawUI();
	ImGui::End();
	}

	if (m_skeletalAnimationFactoryVisible)
	{
	ImGui::BeginChild("SkeletalAnimation factory");
	getSkeletalAnimationFactory().drawUI();
	ImGui::End();
	}*/
}