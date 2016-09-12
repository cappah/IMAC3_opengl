#pragma once

#include "FileHandling\NotifyDirCheck.h"
#include "FileHandling\FileInformation.h"

class ResourceTree;

//windows
#ifdef _WIN32

class DirectoryCheck : public CNotifyDirCheck
{
private:
	ResourceTree* m_resourceTree;

public:
	//DirectoryCheck(ResourceTree* resourceTree);
	virtual UINT Action(CFileInformation fiObject, EFileAction faAction) override;
};

#endif