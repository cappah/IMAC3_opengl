#include "Editor.h"
#include "EditorWindows.h"

DroppedFileEditorWindow* DroppedFileEditorWindow::modalRef = nullptr;

DroppedFileEditorWindow::DroppedFileEditorWindow(Editor* editorRef) 
	: EditorWindow("DroppedFilesPopUp")
	, m_editorRef(editorRef)
{

}

void DroppedFileEditorWindow::openPopUp(Editor& editor)
{
	auto modaleSharedRef = std::make_shared<DroppedFileEditorWindow>(&editor);
	modalRef = modaleSharedRef.get();
	modalRef->treeView.setModel(editor.getResourceTree());
	editor.addModal(modaleSharedRef);
}

void DroppedFileEditorWindow::closePopUp(Editor& editor)
{
	if (!modalRef)
		return;

	editor.removeModal(modalRef);
	modalRef = nullptr;
}

void DroppedFileEditorWindow::drawContent()
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
	treeView.drawContent();
	ImGui::EndChild();

	ImGui::Columns(1);

	ImGui::Separator();

	if (ImGui::Button("Done"))
	{
		m_editorRef->clearDroppedFiles();
		ImGui::CloseCurrentPopup();
		m_shouldCloseModale = true;
	}
}