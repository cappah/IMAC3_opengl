#pragma once

#include <memory>
#include "ResourceTree.h"

//forwards
class Editor;

class DroppedFileEditorWindow : public EditorWindow
{
private:
	ResourceTreeView treeView;
	static DroppedFileEditorWindow* modalRef;
	Editor* m_editorRef;

public:
	DroppedFileEditorWindow(Editor* editorRef);

	static void openPopUp(Editor& editor);
	static void closePopUp(Editor& editor);
	void drawContent() override;
};

/////////////////////////////////////////