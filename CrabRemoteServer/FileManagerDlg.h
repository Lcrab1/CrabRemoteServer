#pragma once
#include"IocpServer.h"
#include"TrueColorToolBar.h"
#include"FileTransferModeDlg.h"
#include"Common.h"
// CFileManagerDlg 对话框
#define MAKEINT64(a, b) ((unsigned __int64)(((DWORD)(a)) | ((unsigned __int64)((DWORD)(b))) << 32))
#define MAX_SEND_BUFFER 0x2000
typedef struct
{
	DWORD	FileSizeHigh;
	DWORD	FileSizeLow;
}FILE_SIZE;
typedef CList<CString, CString&> List;
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

	BYTE	         m_ClientData[0x1000];   //从客户端来的数据
	BYTE             m_ServerData[0x1000];
	PCONTEXT_OBJECT m_ContextObject;
	static ULONG   m_TransferMode;
	BOOL       m_IsDragging;   //拖拽文件
	BOOL       m_IsStop;       //拷贝文件的过程中可以停止
	CIocpServer* m_IocpServer;

	HICON m_IconHwnd;
	HCURSOR m_CursorHwnd;
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


	CListCtrl* m_Drag;
	CListCtrl* m_Drop;
	
	__int64  m_Counter;  //进度条
	__int64     m_TransferFileLength;      //当前操作文件总大小
	CString     m_SourceFullPath;        //拷贝文件
	CString     m_DestinationFullPath;
	List        m_ServerFileToClientJob;   //所有传输任务


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
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLvnBegindragServerFileList(NMHDR* pNMHDR, LRESULT* pResult);


	VOID DropFileOnList();
	VOID CopyServerFileToClient();
	BOOL FixedServerFileToClient(LPCTSTR DircetoryFullPath);
	void EnableControl(BOOL IsEnable);
	BOOL SendServerFileInformationToClient();
	VOID SendServerFileDataToClient();
	void ShowProgress();
	VOID SendTransferMode();
	VOID EndCopyServerFileToClient();


};
