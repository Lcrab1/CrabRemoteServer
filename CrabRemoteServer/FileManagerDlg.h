#pragma once
#include"IocpServer.h"
#include"TrueColorToolBar.h"
// CFileManagerDlg 对话框

class CFileManagerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileManagerDlg)

public:
	CFileManagerDlg(CWnd* pParent = NULL, CIocpServer* IocpServer = NULL, PCONTEXT_OBJECT ContextObject = NULL);
	virtual ~CFileManagerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_MANAGER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CIocpServer* m_IocpServer;
	PCONTEXT_OBJECT m_ContextObject;
	HICON m_IconHwnd;
	BYTE	         m_ClientData[0x1000];   //从客户端来的数据
	BYTE             m_ServerData[0x1000];

	CString m_ServerFullPath;
	CString m_ClientFullPath;

	CString m_ServerFileFullPath;
	CString m_ClientFileFullPath;

	//界面相关
	CTrueColorToolBar m_ServerFileToolBar; //两个工具栏
	CTrueColorToolBar m_ClientFileToolBar;
	CImageList* m_LargeImageList;
	CImageList* m_SmallImageList;
	CStatusBar     m_StatusBar;
	CProgressCtrl* m_ProgressCtrl;


	CListCtrl m_ServerFileList;
	CListCtrl m_ClientFileList;
	CComboBox m_ServerFileCombo;
	CComboBox m_ClientFileCombo;
	CStatic m_ServerPosition;
	CStatic m_ClientPosition;

	BOOL       m_IsDragging = FALSE;   //拖拽文件
	BOOL       m_IsStop = FALSE;       //拷贝文件的过程中可以停止

	void OnReceiveComplete(void);
	int	GetIconIndex(LPCTSTR VolumeName, DWORD FileAttributes);
	void FixedServerVolumeList();
	void FixedClientVolumeList();
	VOID FixedServerFileList(CString DirectoryFullPath = "");
	VOID FixedClientFileList(BYTE* BufferData, ULONG BufferLength);
	CString GetParentDirectory(CString FileFullPath);
	VOID GetClientFileList(CString DirectoryFullPath = "");
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkServerFileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkClientFileList(NMHDR* pNMHDR, LRESULT* pResult);
};
