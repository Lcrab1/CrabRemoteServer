// FileNewFolder.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "FileNewFolder.h"
#include "afxdialogex.h"


// CFileNewFolder 对话框

IMPLEMENT_DYNAMIC(CFileNewFolder, CDialogEx)

CFileNewFolder::CFileNewFolder(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILE_NEW_FOLDER_DIALOG, pParent)
	, m_NewFolderEdit(_T(""))
{

}

CFileNewFolder::~CFileNewFolder()
{
}

void CFileNewFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_NEW_FOLDER_EDIT, m_NewFolderEdit);
	//DDX_Control(pDX, IDC_NEW_FOLDER_EDIT, m_NewFolderEdit);
	DDX_Text(pDX, IDC_NEW_FOLDER_EDIT, m_NewFolderEdit);
}


BEGIN_MESSAGE_MAP(CFileNewFolder, CDialogEx)
END_MESSAGE_MAP()


// CFileNewFolder 消息处理程序
