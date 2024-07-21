#pragma once


// CFileNewFolder 对话框

class CFileNewFolder : public CDialogEx
{
	DECLARE_DYNAMIC(CFileNewFolder)

public:
	CFileNewFolder(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFileNewFolder();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_NEW_FOLDER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	CEdit m_NewFolderEdit;
	CString m_NewFolderEdit;
};
