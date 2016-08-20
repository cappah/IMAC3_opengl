#pragma once

#include <string>

#include "imgui/imgui.h"

#include "imgui_extension.h"
#include "ISingleton.h"
#include "GUI.h"
#include "Utils.h"
#include "Factories.h"

class ResourceFolder;
class ResourceFile;
class ResourceTreeWindow;

enum EditorDragAndDropType
{
	ResourceDragAndDrop = 1<<0,
	ResourceFolderDragAndDrop = 1<<1,
};
ENUM_MASK_IMPLEMENTATION(EditorDragAndDropType);

enum EditorDropContext
{
	DropIntoFileOrFolder = 1 << 0,
	DropIntoResourceField = 1 << 1,
};
ENUM_MASK_IMPLEMENTATION(EditorDropContext);

class EditorWindow
{
public:
	virtual void drawUI() = 0;
};

class ResourceDragAndDropOperation : public DragAndDropOperation
{
private:
	ResourceFolder* m_folderResourceBelongsTo;
	const ResourceFile* m_resourceDragged;
public:
	ResourceDragAndDropOperation(const ResourceFile* resource, ResourceFolder* resourceFolder);
	virtual void dragOperation() override;
	virtual void dropOperation(void* customData, int dropContext) override;
	virtual void cancelOperation() override;
	virtual void updateOperation() override;
	virtual bool canDropInto(void* data) override;
};

class ResourceFolderDragAndDropOperation : public DragAndDropOperation
{
private:
	ResourceTreeWindow* m_resourceTree;
	ResourceFolder* m_parentFolder;
	const ResourceFolder* m_folderDragged;
public:
	ResourceFolderDragAndDropOperation(const ResourceFolder* folder, ResourceFolder* parentFolder, ResourceTreeWindow* resourceTree);
	virtual void dragOperation() override;
	virtual void dropOperation(void* customData, int dropContext) override;
	virtual void cancelOperation() override;
	virtual void updateOperation() override;
};

namespace EditorGUI {

bool ResourceField(ResourceType resourceType, const std::string& label, char* buf, int bufSize);

}
