#include "stdafx.h"

//#include "ResourceTree.h"
#include "DirectoryCheck.h"

#include "FileHandling\NotifyDirCheck.h"
#include "FileHandling\FileInformation.h"

//
//DirectoryCheck::DirectoryCheck(ResourceTree * resourceTree)
//	: m_resourceTree(resourceTree)
//{
//}

UINT DirectoryCheck::Action(CFileInformation fiObject, EFileAction faAction)
{
	CString csBuffer;
	CString csFile = fiObject.GetFilePath();

	if (IS_CREATE_FILE(faAction))
	{
		std::cout << "file created" << std::endl;
		//m_resourceTree->addFile((LPCTSTR)csFile);
	}
	else if (IS_DELETE_FILE(faAction))
	{
		std::cout << "file deleted" << std::endl;
		//m_resourceTree->removeFile((LPCTSTR)csFile);
	}
	else if (IS_CHANGE_FILE(faAction))
	{
		std::cout << "file changed" << std::endl;
		//m_resourceTree->updateFile((LPCTSTR)csFile);
	}
	else
		return 1; //error, stop thread

	return 0; //success
}
